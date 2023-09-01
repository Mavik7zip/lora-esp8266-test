<script>
    import { Bar } from 'svelte-chartjs';

    export const data = {
    labels: [1, 2, 3, 4, 5, 6],
    datasets: [
      {
        data: [12, 19, 3, 5, 2, 3],
      },
    ],
  };
  
    import {
      Chart,
      Title,
      Tooltip,
      Legend,
      BarElement,
      CategoryScale,
      LinearScale,
    } from 'chart.js';
  
    Chart.register(
      Title,
      Tooltip,
      Legend,
      BarElement,
      CategoryScale,
      LinearScale
    );
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
            <select name="mod" id="mod">
              <option value="1">reciver</option>
              <option value="2">sender</option>
              <option value="3">bidirectional</option>
              <option value="4">bidirec-message</option>
            </select>
          </div>
        </div>
        <div class="chart-square">
          <Bar {data} options={{ responsive: true }} />
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