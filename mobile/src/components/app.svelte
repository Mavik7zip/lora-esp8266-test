<App { ...f7params }>

  <!-- Your main view, should have "view-main" class -->
  <View main class="safe-areas" url="/" />

  <Panel left cover>
    <View>
      <Page>
        <Navbar title="Settings"></Navbar>
        <Block>
          <List>
            <form on:submit|preventDefault={post}>
              <ListInput label="TXPower" input={false}>
                <span slot="input">
                  <div class="slider">
                    <Range
                      min={0}
                      max={20}
                      label={true}
                      step={1}
                      bind:value={$settings.txpower}
                      scale={true}
                      scaleSteps={5}
                      scaleSubSteps={20}
                    />
                  </div>
                </span>
              </ListInput>
              <ListInput label="Gain" input={false}>
                <span slot="input">
                  <div class="slider">
                    <Range
                      min={0}
                      max={6}
                      label={true}
                      step={1}
                      bind:value={$settings.gain}
                      scale={true}
                      scaleSteps={6}
                      scaleSubSteps={6}
                    />
                  </div>
                </span>
              </ListInput>
              <ListInput label="Bandwidth" input={false}>
                <span slot="input">
                  <div class="slider">
                    <Range
                      min={7.4}
                      max={500}
                      label={true}
                      step={1}
                      bind:value={$settings.bandwidth}
                      scale={true}
                      scaleSteps={1}
                      scaleSubSteps={1}
                    />
                  </div>
                </span>
              </ListInput>
              <input type="submit" >
            </form>
          </List>
        </Block>
      </Page>
    </View>
  </Panel>

</App>
<script>
  import { onMount } from 'svelte';

  import {
    f7,
    f7ready,
    App,
    View,
    Panel,
    Page,
    Navbar,
    Block,
    ListInput,
    Range,
    List,
  } from 'framework7-svelte';


  import routes from '../js/routes';
  // import store from '../js/store';
  import { settings } from '../js/store';
  import { ip } from '../js/store';



  // Framework7 Parameters
  let f7params = {
    name: 'LoRa-Controller', // App name
    theme: 'auto', // Automatic theme detection




    // App store
    // store: store,
    // App routes
    routes: routes,
  };

  onMount(() => {
    f7ready(() => {


      // Call F7 APIs here
    });
  })

  async function post(){
    const res = await fetch($ip.concat("/post_settings"),
      {method: "post",body: JSON.stringify({"bandwidth": $settings.bandwidth, "txpower": $settings.txpower, "gain": $settings.gain})}
    );
  }
</script>

<style>

  .slider {
    position: relative;
    top: -20%;
    /* width: 120%;
    right: 15%; */
  }

</style>