package ltd.icecold.orange.netty.bean;

import com.google.gson.annotations.SerializedName;

import java.util.List;

public class WeatherDataBean {

    @SerializedName("status")
    private String status;
    @SerializedName("count")
    private String count;
    @SerializedName("info")
    private String info;
    @SerializedName("infocode")
    private String infocode;
    @SerializedName("lives")
    private List<LivesDTO> lives;

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public String getCount() {
        return count;
    }

    public void setCount(String count) {
        this.count = count;
    }

    public String getInfo() {
        return info;
    }

    public void setInfo(String info) {
        this.info = info;
    }

    public String getInfocode() {
        return infocode;
    }

    public void setInfocode(String infocode) {
        this.infocode = infocode;
    }

    public List<LivesDTO> getLives() {
        return lives;
    }

    public void setLives(List<LivesDTO> lives) {
        this.lives = lives;
    }

    public static class LivesDTO {
        @SerializedName("province")
        private String province;
        @SerializedName("city")
        private String city;
        @SerializedName("adcode")
        private String adcode;
        @SerializedName("weather")
        private String weather;
        @SerializedName("temperature")
        private String temperature;
        @SerializedName("winddirection")
        private String winddirection;
        @SerializedName("windpower")
        private String windpower;
        @SerializedName("humidity")
        private String humidity;
        @SerializedName("reporttime")
        private String reporttime;
        @SerializedName("temperature_float")
        private String temperatureFloat;
        @SerializedName("humidity_float")
        private String humidityFloat;

        public String getProvince() {
            return province;
        }

        public void setProvince(String province) {
            this.province = province;
        }

        public String getCity() {
            return city;
        }

        public void setCity(String city) {
            this.city = city;
        }

        public String getAdcode() {
            return adcode;
        }

        public void setAdcode(String adcode) {
            this.adcode = adcode;
        }

        public String getWeather() {
            return weather;
        }

        public void setWeather(String weather) {
            this.weather = weather;
        }

        public String getTemperature() {
            return temperature;
        }

        public void setTemperature(String temperature) {
            this.temperature = temperature;
        }

        public String getWinddirection() {
            return winddirection;
        }

        public void setWinddirection(String winddirection) {
            this.winddirection = winddirection;
        }

        public String getWindpower() {
            return windpower;
        }

        public void setWindpower(String windpower) {
            this.windpower = windpower;
        }

        public String getHumidity() {
            return humidity;
        }

        public void setHumidity(String humidity) {
            this.humidity = humidity;
        }

        public String getReporttime() {
            return reporttime;
        }

        public void setReporttime(String reporttime) {
            this.reporttime = reporttime;
        }

        public String getTemperatureFloat() {
            return temperatureFloat;
        }

        public void setTemperatureFloat(String temperatureFloat) {
            this.temperatureFloat = temperatureFloat;
        }

        public String getHumidityFloat() {
            return humidityFloat;
        }

        public void setHumidityFloat(String humidityFloat) {
            this.humidityFloat = humidityFloat;
        }
    }
}
