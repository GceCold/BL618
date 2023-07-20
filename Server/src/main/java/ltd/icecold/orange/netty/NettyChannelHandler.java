package ltd.icecold.orange.netty;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;
import com.google.gson.Gson;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import ltd.icecold.orange.netty.bean.WeatherDataBean;
import ltd.icecold.orange.network.Request;
import ltd.icecold.orange.utils.FfmpegUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Component
public class NettyChannelHandler extends ChannelInboundHandlerAdapter {
    public final ExecutorService NETWORK_EXECUTOR = Executors.newFixedThreadPool(5);
    private static final Logger LOGGER = LoggerFactory.getLogger(NettyChannelHandler.class);
    private static List<Channel> connections = Lists.newArrayList();
    private static final Gson GSON = new Gson();
    private static List<ByteBuf> imageDataList;

//    @Autowired
//    private IRobotStateService robotStateService;

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        ByteBuf byteBuf = (ByteBuf) msg;
        byte[] data = new byte[byteBuf.readableBytes()];
        byteBuf.readBytes(data);
        Channel channel = ctx.channel();
        NETWORK_EXECUTOR.submit(() -> {
            String command = new String(data, StandardCharsets.UTF_8).trim();
            if ("weather".equals(command)) {
                String url = String.format("https://restapi.amap.com/v3/weather/weatherInfo?city=%s&key=100f6b756311c044432b4759359856e0", "110101");
                String weatherJson = null;
                try {
                    weatherJson = Request.sendGet(url, Maps.newHashMap(), Maps.newHashMap()).body();
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
                if (weatherJson == null || "".equals(weatherJson))
                    return;
                WeatherDataBean weatherDataBean = GSON.fromJson(weatherJson, WeatherDataBean.class);
                if (weatherDataBean.getLives() == null || weatherDataBean.getLives().size() == 0)
                    return;
                WeatherDataBean.LivesDTO livesDTO = weatherDataBean.getLives().get(0);
                String sendData = String.format("WH@%s@%s@%s@%s@%s@%s",
                        livesDTO.getProvince() + livesDTO.getCity(),
                        livesDTO.getWeather(),
                        livesDTO.getTemperature(),
                        livesDTO.getWinddirection(),
                        livesDTO.getWindpower().replace("≤", "<").replace("≥", ">"),
                        livesDTO.getHumidity()
                );
                channel.writeAndFlush(Unpooled.wrappedBuffer(sendData.getBytes(StandardCharsets.UTF_8)));
                return;
            }
            if (command.contains("PWM")) {
                String number = command.substring(command.indexOf("PWM") - 1, command.indexOf("PWM"));
                for (Channel value : connections) {
                    if (value.isActive() && !value.equals(channel))
                        value.writeAndFlush(Unpooled.wrappedBuffer(String.format("RT@%s", number.trim()).getBytes(StandardCharsets.UTF_8)));
                }
            }
            if (command.contains("PH_END")) {
                String number;
                ByteBuf byteBuf1 = Unpooled.wrappedBuffer(data);
                byte[] temp = new byte[byteBuf1.readableBytes()];
                number = command.substring(command.indexOf("PH_END") + 6, command.indexOf("PH_END") + 7);

                byteBuf1.readBytes(temp, 0, temp.length);

                imageDataList.add(Unpooled.wrappedBuffer(temp));
                ByteBuf image = Unpooled.wrappedBuffer(imageDataList.toArray(new ByteBuf[0]));
                byte[] imageData = new byte[153600];
                image.readBytes(imageData);
                image.release();
                File file = new File("photo/image-" + number + ".rgb");
                File fileOut = new File("photo/image-" + number + ".jpg");
                if (fileOut.exists())
                    fileOut.delete();
                writeFile(file, imageData);
                imageDataList.clear();
                FfmpegUtils.rgb565beToJpg(file.getAbsolutePath(), fileOut.getAbsolutePath());
                file.delete();
                return;
            } else if (command.contains("UUID")){

                return;
            }

            if (imageDataList == null || imageDataList.size() == 0) {
                imageDataList = new ArrayList<>();
            }
            imageDataList.add(Unpooled.wrappedBuffer(data));
        });
    }

    public static void writeFile(File f, byte[] data) {
        try {
            if (!f.exists()) {
                f.createNewFile();
            }
            BufferedOutputStream boss = new BufferedOutputStream(Files.newOutputStream(f.toPath()));
            boss.write(data);
            boss.flush();
            boss.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        ctx.close();
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        connections.remove(ctx.channel());
        LOGGER.info("Node: " + getAddressWithPort(ctx.channel()) + " disconnected server");
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        LOGGER.info("Node: " + getAddressWithPort(ctx.channel()) + " connected server");
        if (imageDataList != null && imageDataList.size() > 0)
            imageDataList.clear();
        ctx.channel().writeAndFlush("CL@"+ClotheMapper.queryAll());
        connections.add(ctx.channel());
    }

    public static String getAddressWithPort(Channel channel) {
        return channel.remoteAddress().toString().replace("/", "");
    }

}
