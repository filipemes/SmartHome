package com.smarthomeapp;

import android.os.Bundle;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;
import com.smarthomeapp.Entities.Node1;
import com.smarthomeapp.Entities.Node2;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.VideoView;

import java.util.LinkedList;

public class MainActivity extends AppCompatActivity {

    private Button increaseThreshold;
    private Button decreaseThreshold;
    private TextView thresholdTextView;
    private int threshold;
    private ImageView fanImage;
    private Thread fanThread;
    private TextView temperatureTextView;
    private TextView gasTextView;
    private TextView lastAlert;
    private TextView lastWatering;
    private TextView moisturePercentage;
    private TextView roomHumidityPercentage;
    private Switch onOffSwitch;
    private VideoView videoViewStream;
    volatile boolean running = true;
    FirebaseDatabase firebaseDatabase= FirebaseDatabase.getInstance();
    DatabaseReference ref1=firebaseDatabase.getReference("node1");
    DatabaseReference ref2=firebaseDatabase.getReference("node2");
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        roomHumidityPercentage =(TextView)findViewById(R.id.roomHumidityPercentage);
        lastWatering=(TextView)findViewById(R.id.lastWatering);
        moisturePercentage=(TextView)findViewById(R.id.moisturePercentage);
        temperatureTextView=(TextView)findViewById(R.id.temperatureTextView);
        thresholdTextView =(TextView)findViewById(R.id.textViewThreshold);
        gasTextView = (TextView) findViewById(R.id.gasConcentration);
        lastAlert = (TextView) findViewById(R.id.lastAlert);
        fanImage=(ImageView)findViewById(R.id.fan_image);
        threshold=25;
        increaseThreshold=(Button)findViewById(R.id.increaseThreshold);
        decreaseThreshold=(Button)findViewById(R.id.decreaseThreshold);
        onOffSwitch = (Switch)  findViewById(R.id.switchFan);
        onOffSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    running=true;
                    ref1.child("fanState").setValue("true");
                    moveFan();
                }
                else {
                    ref1.child("fanState").setValue("false");
                    running=false;
                }
            }

        });
        increaseThreshold.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                threshold++;
                ref1.child("threshold").setValue(threshold);
                setThresholdTextView();
            }
        });
        decreaseThreshold.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                threshold--;
                ref1.child("threshold").setValue(threshold);
                setThresholdTextView();
            }
        });
        ref1.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                Node1 value = dataSnapshot.getValue(Node1.class);
                threshold=value.getThreshold();
                setThresholdTextView();
                setTemperatureTextView(value.getTemperature());
                setGasConcentrationTextView(value.getGasConcentration());
                setLastAlert(value.getLastAlert());
                setRoomHumidityPercentage(value.getHumidityPercentage());
                setFanState(Boolean.valueOf(value.getFanState()));
            }
            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {
            }
        });
        ref2.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                Node2 value = dataSnapshot.getValue(Node2.class);
                moisturePercentage.setText("Soil moisture\n percentage: "+value.getMoisturePercentage()+"%");
                lastWatering.setText("Last Watering: "+value.getLastWatering());
            }
            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {
            }
        });

        videoViewStream = (VideoView) this.findViewById(R.id.videoViewStream);
        videoViewStream.setVideoPath("rtsp://172.16.1.6:8554/stream");
        videoViewStream.requestFocus();
        videoViewStream.start();
    }

    private void setRoomHumidityPercentage(int moisturePercentage) {
        this.roomHumidityPercentage.setText(moisturePercentage+"%");
    }

    public void setGasConcentrationTextView(int conc) {
        this.gasTextView.setText("Gas Concentration: "+conc);
    }

    public void setLastAlert(String str) {
        this.lastAlert.setText("Last Alert: "+str);
    }

    public void setTemperatureTextView(int temp) {
        this.temperatureTextView.setText(temp+"º C");
    }

    public void setFanState(boolean state){
        this.onOffSwitch.setChecked(state);
    }

    private void moveFan(){
        fanThread=new Thread(new Runnable() {
            LinkedList<Integer> values=new LinkedList<>();
            int i=0;
            @Override
            public void run() {
                values.add(R.drawable.fan_1);
                values.add(R.drawable.fan_2);
                values.add(R.drawable.fan_3);
                while(running){
                    if(i==3) i=0;
                    synchronized (this) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                switch (i){
                                    case 0:
                                        fanImage.setImageResource(values.get(0));
                                        break;
                                    case 1:
                                        fanImage.setImageResource(values.get(1));
                                        break;
                                    case 2:
                                        fanImage.setImageResource(values.get(2));
                                        break;
                                }

                                fanImage.invalidate();
                            }
                        });
                        i++;
                    }

                    try {
                        Thread.sleep(50);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        fanThread.start();
    }
    private void setThresholdTextView(){
        this.thresholdTextView.setText(Integer.toString(this.threshold));
    }


}
