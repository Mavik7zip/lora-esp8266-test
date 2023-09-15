<script>

    import {
      Page,
      Icon,
      Navbar,
      NavTitle,
      NavTitleLarge,
      Link,
      Toolbar,
      Block,
      Panel,
      Button,
      ListInput,
      Range,
    } from 'framework7-svelte';
  
    import { onMount } from 'svelte';
    import { Line } from 'svelte-chartjs';
    import { CapacitorHttp } from '@capacitor/core';
  
    import {
      Chart,
      Title,
      Tooltip,
      Legend,
      LineElement,
      LinearScale,
      PointElement,
      CategoryScale,
    } from 'chart.js';

  
    Chart.register(
      Title,
      Tooltip,
      Legend,
      LineElement,
      LinearScale,
      PointElement,
      CategoryScale
    );
  
    import { settings } from '../js/store';
    import { ip } from '../js/store';
  
  
    let packet = {"rssi_radio": 0, "rssi": 0, "snr": 0, "text": 0, "ping": 0, "counter": 0, "is_arrive": false, "mod": 0,};
    let rssi=[];
    let snr=[];
    let counter = 0;
    let timestamp=[];
    let chartRef;
    let mod;
  
  
  
    onMount( async () => {
      const options = {
        url: $ip.concat("/settings"),
      };
      const response = await CapacitorHttp.get(options);
      $settings = response.data;
    });
  
  
    setInterval(async function() {
  
      const options = {
        url: $ip.concat("/get"),
      };
      const response = await CapacitorHttp.get(options);
      packet = response.data;
      
  
      rssi.push(packet.rssi);
      snr.push(packet.snr);
  
      if(rssi.length > 16){
        rssi.shift();
        snr.shift();
        timestamp.shift();
      }
  
      timestamp.push(counter);
      counter++;
  
      if(chartRef != null){
        chartRef.update();
      }
      
    }, 1000); 
  
    export const data = {
      labels: timestamp,
      datasets: [{
          data: snr,
        },
      ],
    };
  
    async function post(){
      const options = {
        url: $ip.concat("/post"),
        data: { "mod": mod },
      };
  
      const response = await CapacitorHttp.post(options);
    }
  
  </script>
  
  <Page name="home">
    <body>
        <div class="content-box">
            <div class="grid-navbar">
                <h1 color="withe">LoRa Controller</h1>
                <div class="ciro">
                    <img src="settings_white_24dp.svg" alt="settings" width="120%">
                </div>
            </div>
            <div class="rectangle">
                    50%
                </div>
            <div class="grid grid-cols-2 grid-gap">
                <div class="rectangle">
                    50%
                </div>
                <div class="rectangle">
                    50%
                </div>
            </div>
        </div>
    </body>
  </Page>
  
            <!-- <form on:submit|preventDefault={post}>
              <select name="mod" id="mod" bind:value={mod}>
                <option value="1">reciver</option>
                <option value="2">sender</option>
                <option value="3">bidirectional</option>
                <option value="4">bidirec-message</option>
              </select>
              <input type="submit" >
            </form> -->

  
  <style>
    body {
        background-color: black;
    }

    .content-box {
        width: 90%;
        height: 90%;
        margin-left: 5%;
    }

    .grid-navbar {
        display: grid;
        grid-template-columns: 90% 10%;
    }

    .ciro {
        position: relative;
        top: 20%;
    }

    .rectangle {
        width: 100%;
        height: 120px;
        border-radius: 20%;
        background-color: #0f0f0f;
    }
  </style>