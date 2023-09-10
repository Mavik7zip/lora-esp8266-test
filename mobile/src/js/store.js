import { writable } from 'svelte/store';

export let settings = writable({"bandwidth": 0, "txpower": 0, "gain": 0});
export const ip = writable("http://192.168.100.166");