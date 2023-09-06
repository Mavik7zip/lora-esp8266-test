<script>
    import { Bar } from 'svelte-chartjs';
    import { onMount } from 'svelte';

  
    import {
      Chart,
      Title,
      Tooltip,
      Legend,
      BarElement,
      LineElement,
      CategoryScale,
      LinearScale,
    } from 'chart.js';
  
    Chart.register(
      Title,
      Tooltip,
      Legend,
      BarElement,
      LineElement,
      CategoryScale,
      LinearScale
    );

    let packet;
    let rssi=[];
    let snr=[];
    let counter = 0;
    let timestamp=[];
    let chartRef;
    let mod;

    const ip = "http://192.168.100.166";


    setInterval(async function() {
      const res = await fetch(ip.concat("/get"));
      packet = await res.json();

      rssi.push(packet.rssi);
      snr.push(packet.snr);

      if(rssi.length > 16){
        rssi.shift();
        snr.shift();
      }

      timestamp.push(counter);
      counter++;

      if(chartRef != null){
        chartRef.update();
      }
      
    }, 5000); 

    export const data = {
      labels: timestamp,
      datasets: [{
          data: snr,
        },
      ],
    };

  async function post(){
    const res = await fetch(ip.concat("/post"), {method: "post",body: JSON.stringify({"mod": mod})});

  }

  </script>
  
  <main>
    <body>
      <div class="top-grid">
        <div>
          <div class="settings-square">
            <div style="top: 8px;">
              <div>BWD<span id="bandwidth"></span></div>
              <div>TXP<span id="TXpower"></span></div>
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
          <Bar {data}bind:chart={chartRef} options={{ responsive: true }} />
        </div>
      </div>
      <div class="text-box">
        <div>ciro</div>
      </div>
    </body>
  </main>
  

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
</style>