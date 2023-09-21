<script>

  import {
    Page,
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
    // netScan.ipScan('192.168.100.0-254', host => {
    //   console.log(host);
    // })

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
      {
        data: rssi,
      },
    ],
  };

  async function post(){
    const options = {
      url: $ip.concat("/post"),
      data: { "mod": mod },
      headers: { "content-type": "application/json" },
    };

    await CapacitorHttp.post(options);
  }

</script>

<Page name="home">
  <body>
    <div class="top-grid">
      <div>
        <div class="settings-square">
          <div style="top: 8px;">
            <div>BWD {$settings.bandwidth}khz</div>
            <div>TXP {$settings.txpower}dbm</div>
            <div>RSR {packet.rssi_radio}dbm</div>
          </div>
        </div>
        <div class="mod-box">
          <form on:submit|preventDefault={post}>
            <select name="mod" id="mod" bind:value={mod}>
              <option value="1">reciver</option>
              <option value="2">sender</option>
              <option value="3">bidirectional</option>
              <option value="4">bidirec-message</option>
            </select>
            <input type="submit" >
          </form>
        </div>
      </div>
      <div class="chart-square">
        <Line {data}bind:chart={chartRef} options={{ responsive: true }}/>
      </div>
    </div>
    <div class="text-box">
      <div>
        <Button raised fill panelOpen="left">Open left panel</Button>
        <div class="packet-box">
          <div>snr {packet.snr}db</div>
          <div>rssi {packet.rssi}dbm</div>
          <div>text {packet.text}</div>
          <div>N° {packet.counter}</div>
        </div>
      </div>
    </div>
  </body>
</Page>


<style>
  .settings-square{
    width: 100%;
    height: 120px;
    background-color: yellow;
  }

  .chart-square {
    width: 100%;
    height: 100%;
    background-color: rgb(190, 92, 190);
  }

  .top-grid {
    display: grid;
    grid-template-columns: 40% 60%;
  }

  .mod-box {
    width: 100%;
    height: 40px;
    background-color: palevioletred;
  }

  .text-box {
    width: 100%;
    height: 360px;
    background-color: blue;
  }

  body{
    margin: 0px;
  }

  .packet-box {
    padding: 10%;
  }
</style>