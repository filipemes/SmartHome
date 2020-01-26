package com.smarthomeapp.Entities;

import android.os.Parcel;
import android.os.Parcelable;

public class Node2  implements Parcelable {
    private String lastWatering;
    private int moisturePercentage;


    public Node2(){

    }
    protected Node2(Parcel in) {
        this.lastWatering=in.readString();
        this.moisturePercentage=in.readInt();
    }

    public String getLastWatering() {
        return lastWatering;
    }

    public int getMoisturePercentage() {
        return moisturePercentage;
    }

    public static final Creator<Node2> CREATOR = new Creator<Node2>() {
        @Override
        public Node2 createFromParcel(Parcel in) {
            return new Node2(in);
        }

        @Override
        public Node2[] newArray(int size) {
            return new Node2[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(lastWatering);
        parcel.writeInt(moisturePercentage);
    }
}
