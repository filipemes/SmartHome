package com.smarthomeapp.Entities;

import android.os.Build;
import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.RequiresApi;


public class Node1 implements Parcelable {
    private String fanState;
    private int gasConcentration;
    private String lastAlert;
    private int temperature;
    private int threshold;
    private int humidityPercentage;

    public String getFanState() {
        return fanState;
    }

    public int getGasConcentration() {
        return gasConcentration;
    }

    public String getLastAlert() {
        return lastAlert;
    }

    public int getTemperature() {
        return temperature;
    }

    public int getThreshold() {
        return threshold;
    }

    public int getHumidityPercentage() {
        return humidityPercentage;
    }

    public static Creator<Node1> getCREATOR() {
        return CREATOR;
    }

    public Node1(String fanState, int gasConcentration, String lastAlert, int temperature, int threshold,int humidityPercentage) {
        this.fanState = fanState;
        this.gasConcentration = gasConcentration;
        this.lastAlert = lastAlert;
        this.temperature = temperature;
        this.threshold = threshold;
        this.humidityPercentage=humidityPercentage;
    }

    public Node1(){

    }
    @RequiresApi(api = Build.VERSION_CODES.Q)
    protected Node1(Parcel in) {
        this.fanState = in.readString();
        this.gasConcentration = in.readInt();
        this.lastAlert =in.readString();
        this.temperature = in.readInt();
        this.threshold = in.readInt();
        this.humidityPercentage=in.readInt();
    }


    public static final Creator<Node1> CREATOR = new Creator<Node1>() {
        @RequiresApi(api = Build.VERSION_CODES.Q)
        @Override
        public Node1 createFromParcel(Parcel in) {
            return new Node1(in);
        }

        @Override
        public Node1[] newArray(int size) {
            return new Node1[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public String toString() {
        return "Node1{" +
                "fanState='" + fanState + '\'' +
                ", gasConcentration=" + gasConcentration +
                ", lastAlert='" + lastAlert + '\'' +
                ", temperature=" + temperature +
                ", threshold=" + threshold +
                ", moisturePercentage =" + humidityPercentage +
                '}';
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(fanState);
        parcel.writeInt(gasConcentration);
        parcel.writeString(lastAlert);
        parcel.writeInt(temperature);
        parcel.writeInt(threshold);
        parcel.writeInt(humidityPercentage);
    }
}
