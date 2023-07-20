package ltd.icecold.orange.netty;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;

public class ImageUtils {
    private static final String TAG = "ImageUtils";

    public static boolean saveRawImage(String sFileName, byte[] baImage) {
        try {
            File file = new File(sFileName);
            if (file.exists()) {
                file.delete();
            }
            file.createNewFile();

            OutputStream outStream = new FileOutputStream(file);
            outStream.write(baImage);
            outStream.flush();
            outStream.close();

            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    private static int[] ExtractPixelData(byte[] baYuvData, int fullwidth, int iCropTop, int iCropLeft, int iCropWidth, int iCropHeight) {
        int[] pixels = new int[iCropWidth * iCropHeight];
        int inputOffset = iCropTop * fullwidth + iCropLeft;

        for (int y = 0; y < iCropHeight; y++) {
            int outputOffset = y * iCropWidth;
            for (int x = 0; x < iCropWidth; x++) {
                int grey = baYuvData[(inputOffset + x)] & 0xFF;
                pixels[(outputOffset + x)] = (0xFF000000 | grey * 65793);
            }

            inputOffset += fullwidth;
        }

        return pixels;
    }

    private byte[] rotateYUVDegree90(byte[] data, int imageWidth, int imageHeight) {
        byte[] yuv = new byte[imageWidth * imageHeight * 3 / 2];
        // Rotate the Y luma
        int i = 0;
        for (int x = 0; x < imageWidth; x++) {
            for (int y = imageHeight - 1; y >= 0; y--) {
                yuv[i] = data[y * imageWidth + x];
                i++;
            }
        }
        // Rotate the U and V color components
        i = imageWidth * imageHeight * 3 / 2 - 1;
        for (int x = imageWidth - 1; x > 0; x = x - 2) {
            for (int y = 0; y < imageHeight / 2; y++) {
                yuv[i] = data[(imageWidth * imageHeight) + (y * imageWidth) + x];
                i--;
                yuv[i] = data[(imageWidth * imageHeight) + (y * imageWidth) + (x - 1)];
                i--;
            }
        }
        return yuv;
    }

    private byte[] rotateYUVDegree270(byte[] data, int imageWidth, int imageHeight) {
        byte[] yuv = new byte[imageWidth * imageHeight * 3 / 2];
        // Rotate the Y luma
        int i = 0;
        for (int x = imageWidth - 1; x >= 0; x--) {
            for (int y = 0; y < imageHeight; y++) {
                yuv[i] = data[y * imageWidth + x];
                i++;
            }
        }// Rotate the U and V color components
        i = imageWidth * imageHeight;
        for (int x = imageWidth - 1; x > 0; x = x - 2) {
            for (int y = 0; y < imageHeight / 2; y++) {
                yuv[i] = data[(imageWidth * imageHeight) + (y * imageWidth) + (x - 1)];
                i++;
                yuv[i] = data[(imageWidth * imageHeight) + (y * imageWidth) + x];
                i++;
            }
        }
        return yuv;
    }

    private byte[] rotateYUVDegree270AndMirror(byte[] data, int imageWidth, int imageHeight) {
        byte[] yuv = new byte[imageWidth * imageHeight * 3 / 2];
        // Rotate and mirror the Y luma
        int i = 0;
        int maxY = 0;
        for (int x = imageWidth - 1; x >= 0; x--) {
            maxY = imageWidth * (imageHeight - 1) + x * 2;
            for (int y = 0; y < imageHeight; y++) {
                yuv[i] = data[maxY - (y * imageWidth + x)];
                i++;
            }
        }
        // Rotate and mirror the U and V color components
        int uvSize = imageWidth * imageHeight;
        i = uvSize;
        int maxUV = 0;
        for (int x = imageWidth - 1; x > 0; x = x - 2) {
            maxUV = imageWidth * (imageHeight / 2 - 1) + x * 2 + uvSize;
            for (int y = 0; y < imageHeight / 2; y++) {
                yuv[i] = data[maxUV - 2 - (y * imageWidth + x - 1)];
                i++;
                yuv[i] = data[maxUV - (y * imageWidth + x)];
                i++;
            }
        }
        return yuv;
    }

    /*byte[] RGBbyteArray = frameData.array();
    // allocate buffer to store bitmap RGB pixel data
    int[] rgb = convertByteToIntArray(RGBbyteArray);*/
    // 将一个byte数转成int
    // 实现这个函数的目的是为了将byte数当成无符号的变量去转化成int
    public static int convertByteToInt(byte data) {

        int heightBit = (int) ((data >> 4) & 0x0F);
        int lowBit = (int) (0x0F & data);
        return heightBit * 16 + lowBit;
    }


    // 将纯RGB数据数组转化成int像素数组
    public static int[] convertByteToIntArray(byte[] data) {
        int size = data.length;
        if (size == 0) return null;

        int arg = 0;
        if (size % 3 != 0) {
            arg = 1;
        }
        // 一般RGB字节数组的长度应该是3的倍数，
        // 不排除有特殊情况，多余的RGB数据用黑色0XFF000000填充
        int[] color = new int[size / 3 + arg];
        int r, g, b;
        int colorLen = color.length;
        if (arg == 0) {
            for (int i = 0; i < colorLen; ++i) {
                r = convertByteToInt(data[i * 3]);
                g = convertByteToInt(data[i * 3 + 1]);
                b = convertByteToInt(data[i * 3 + 2]);
                // 获取RGB分量值通过按位或生成int的像素值
                color[i] = (r << 16) | (g << 8) | b | 0xFF000000;
            }
        } else {
            for (int i = 0; i < colorLen - 1; ++i) {
                r = convertByteToInt(data[i * 3]);
                g = convertByteToInt(data[i * 3 + 1]);
                b = convertByteToInt(data[i * 3 + 2]);
                color[i] = (r << 16) | (g << 8) | b | 0xFF000000;
            }

            color[colorLen - 1] = 0xFF000000;
        }

        return color;
    }

    private int[] rgb24ToPixel(byte[] rgb24, int width, int height) {
        int[] pix = new int[rgb24.length / 3];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int idx = width * i + j;
                int rgbIdx = idx * 3;
                int red = rgb24[rgbIdx];
                int green = rgb24[rgbIdx + 1];
                int blue = rgb24[rgbIdx + 2];
                int color = (blue & 0x000000FF) | (green << 8 & 0x0000FF00) | (red << 16 & 0x00FF0000);
                pix[idx] = color;
            }
        }
        return pix;
    }

    private byte[] pixelToRgb24(int[] pix, int width, int height) {
        byte[] rgb24 = new byte[width * height * 3];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int idx = width * i + j;
                int color = pix[idx]; //获取像素
                int red = ((color & 0x00FF0000) >> 16);
                int green = ((color & 0x0000FF00) >> 8);
                int blue = color & 0x000000FF;

                int rgbIdx = idx * 3;
                rgb24[rgbIdx] = (byte) red;
                rgb24[rgbIdx + 1] = (byte) green;
                rgb24[rgbIdx + 2] = (byte) blue;
            }
        }
        return rgb24;
    }
}