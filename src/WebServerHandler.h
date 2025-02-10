#pragma once
#include <Arduino.h>
#include <utils.h>
#include <Config.h>
#ifdef ESP32
#include <AsyncTCP.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(TARGET_RP2040)
#include <WebServer.h>
#include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>

namespace WebServerHandler
{
    static uint32_t lastHeap = 0;
    static AsyncWebServer server(80);
    static AsyncWebSocket ws("/ws");

    static const char *htmlContent PROGMEM = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <link rel="manifest" href="/app.webmanifest">
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Mushroom lamp</title>

            <style>
                @import url('https://fonts.googleapis.com/css2?family=Itim&display=swap');

                * {
                    margin: 0;
                    padding: 0;
                    box-sizing: border-box;
                }

                body {
                    background-color: #211e33;
                    min-height: 100dvh;
                    color: #fcddd8;
                    font-family: "Itim", system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
                }

                .loader {
                    position: fixed;
                    inset: 0;
                    background-color: #211e33;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    font-size: 1.5rem;
                }

                .loader.hidden {
                    display: none;
                }

                .mushroom_container {
                    display: flex;
                    align-items: center;
                    justify-content: center;
                }

                .mushroom {
                    width: 80%;
                    padding-block: 2rem;
                }

                .mushroom .eye, .mushroom .mouth, .mushroom .tounge {
                    transform-box: border-box;
                    transform-origin: center center;
                    transform: scaleY(100%);
                    transition: transform 100ms ease-in-out, fill 100ms ease-in-out;
                }

                .mushroom.off .eye {
                    transform: scaleY(30%) scaleX(120%);
                }

                .mushroom.off .mouth {
                    transform: scale(30%) scaleY(50%);
                    stroke-width: 100px;
                }

                .mushroom.off .tounge {
                    transform: scale(0%);
                }

                .mushroom #b {
                    transition: fill 100ms ease-in-out;
                }
                .mushroom.off #b {
                    fill: #231c43!important;
                }

                .controls {
                    width: 80%;
                    margin: auto;
                }

                .input-group {
                    display: flex;
                    flex-direction: column;
                    gap: 0.3rem;
                }

                .input-group:not(:last-of-type) {
                    margin-bottom: 2rem;
                }

                .input-group label {
                    font-size: 1.2rem;
                }

                input[type=color] {
                    border: 3px solid #fcddd8;
                    width: 100%;
                    border-radius: 6px;
                }

                input[type=range] {
                    accent-color: #fcddd8;
                }

                select {
                    background-color: #fcddd8;
                    border: 3px solid #fcddd8;
                    border-radius: 6px;
                    padding: 0.2rem 0.5rem;
                    font-size: 1rem;
                    font-family: inherit;
                    color: #0d0d28;
                }

                svg {
                    overflow: visible;

                    animation-duration: 650ms;
                    transform-origin: bottom center;

                    animation-timing-function: linear(0, 0.013 0.6%, 0.051 1.2%, 0.201 2.5%, 0.398 3.7%, 0.957 6.7%, 1.21 8.4%, 1.298 9.2%, 1.363 10%, 1.404 10.8%, 1.423 11.6%, 1.423 12.2%, 1.412 12.8%, 1.351 14.2%, 1.263 15.5%, 1.011 18.7%, 0.905 20.4%, 0.846 21.9%, 0.821 23.4%, 0.823 24.5%, 0.842 25.7%, 1.029 31.8%, 1.063 33.6%, 1.076 35.3%, 1.067 37.6%, 0.988 43.7%, 0.968 47.1%, 0.971 49.5%, 1.005 55.5%, 1.014 58.9%, 0.994 70.6%, 1.002 82.4%, 1)
                }

                .mushroom.squish svg {
                    animation-name: squish;
                }

                #c > path, #d > path, #e > path {
                    animation-duration: 8s;
                    animation-name: star;
                    animation-iteration-count: infinite;
                    transform-origin: center center;
                }

                #c > path { animation-delay: 2s; }
                #d > path { animation-duration: 5s; }
                #e > path { animation-delay: 4s; }

                @keyframes squish {
                    0% {
                        transform: scaleX(100%) scaleY(100%);
                    }
                    50% {
                        transform: scaleX(98%) scaleY(95%);
                    }
                    100% {
                        transform: scaleX(100%) scaleY(100%);
                    }
                }

                @keyframes star {
                    0% {
                        scale: 100%;
                    }

                    15% {
                        filter: brightness(0.7);
                    }

                    50% {
                        filter: brightness(1);
                    }

                    60% {
                        scale: 105%;
                    }
                    63% {
                        scale: 105%;
                    }

                    70% {
                        filter: brightness(1.2);
                    }

                    72% {
                        filter: brightness(1);
                    }

                    100% {
                        scale: 100%;
                    }
                }

            </style>

        </head>
        <body>
            <div class="mushroom_container">
                <div class="mushroom">
                    <svg id="a" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 154.87 170.36"><path id="b" d="M19.65,95.06c-8.13-3.45-9.45-4.53-10.57-5.88-1.66-1.99-2.31-3.91-3.17-6.51-.53-1.6-2.4-7.44-1.94-14.13.64-9.21,5.39-15.53,13.35-26.13,5.75-7.65,10.49-11.83,11.81-12.97,4.46-3.86,8.27-6.2,11.42-8.13,6.51-3.97,11.83-7.22,19.55-9.27,7.72-2.05,13.93-1.86,20.71-1.66,5.65.17,10.19.34,15.93,2.08,6.14,1.86,10.26,4.49,14.27,7.1,2.1,1.36,10.84,7.18,19.74,18.01,1.43,1.73,6.28,7.76,11.23,17.03,7.38,13.83,11.87,22.25,8.9,31.16-2.54,7.63-9.26,11.78-10.25,12.37-1.89,1.13-3.33,1.61-12.98,3.5-17.49,3.43-19.93,3.26-21.55,1.62-1.08-1.09-.34-1.36-2.06-11.11-1.66-9.36-1.57-4.71-2.95-12-.46-2.42-.78-4.56-2.52-5.75-1.26-.86-2.51-.71-4.59-.69-3.51.02-6.97-.77-10.46-1.1-7.73-.73-6.65-.73-9.49-.93-4.56-.32-7.26-.5-10.84-.23-4.77.37-7.15.55-9.06,1.82-.76.51-1.97,1.48-6.06,11.79-1.55,3.9-3.58,7.61-4.9,11.59-.71,2.16-1.14,3.77-2.63,4.61-1.84,1.03-3.96.11-7.74-1.35-9.03-3.51-5.8-1.71-13.16-4.84Z" style="fill:#60e2de;"/><g style="opacity:.14;"><path d="M49.91,83.62c-.77-1.55-1.57-3.08-2.32-4.63-.38-.8-.77-1.59-1.13-2.4-.38-.86-.73-1.76-1.53-2.31-.38-.26-1.01-.24-1.18.27-.3.92.06,1.82.44,2.66.4.89.82,1.77,1.31,2.62.95,1.66,2.03,3.12,3.36,4.49.48.49,1.37-.06,1.06-.7h0Z" style="fill:#212121;"/><path d="M46.63,89.49c-1.11-1.04-2.19-2.12-3.26-3.2s-2.24-2.14-3.29-3.28c-.96-1.04-1.83-2.16-2.42-3.46-.29-.63-.52-1.29-.72-1.96-.22-.74-.33-1.35-.94-1.87-.32-.27-.84-.22-1.09.12-.88,1.2.06,3.15.63,4.33.68,1.41,1.57,2.68,2.67,3.79s2.45,2.2,3.73,3.22,2.68,2.04,4.11,2.95c.41.26.99-.28.59-.65h0Z" style="fill:#212121;"/><path d="M102.19,84.64c.4.19.88-.02,1.24-.19.4-.19.78-.39,1.16-.62.78-.48,1.52-1,2.21-1.6,1.32-1.14,2.63-2.5,3.48-4.03.25-.44.1-.95-.36-1.19-.49-.25-1.04-.09-1.38.31-1.13,1.32-2.12,2.61-3.38,3.81-.62.6-1.25,1.18-1.9,1.75-.26.23-1.83,1.41-1.08,1.76h0Z" style="fill:#212121;"/><path d="M103.56,90.06c3.16-.6,6.13-1.72,8.9-3.35,1.34-.79,2.62-1.67,3.84-2.65.62-.5,1.22-1.02,1.8-1.57s1.24-1.11,1.42-1.9c.17-.71-.62-.97-1.17-.77-.72.26-1.24.84-1.79,1.35-.52.49-1.06.95-1.61,1.41-1.16.95-2.39,1.83-3.67,2.62-2.52,1.56-5.25,2.88-8.05,3.86-.73.26-.4,1.14.32,1h0Z" style="fill:#212121;"/><path d="M104.42,95.16c1.89.71,4.17.48,6.13.23,2.29-.28,4.5-.86,6.66-1.68s4.08-1.83,5.94-3.07c.94-.63,1.85-1.32,2.71-2.06.77-.67,1.77-1.4,2.13-2.39.23-.64-.64-.94-1.1-.73-.88.4-1.53,1.21-2.25,1.82-.77.66-1.58,1.28-2.42,1.85-1.76,1.2-3.65,2.21-5.63,3.01s-3.93,1.33-5.96,1.7-4.14.26-6.11.76c-.25.06-.38.44-.08.55h0Z" style="fill:#212121;"/><path d="M105.35,99c3.91,1.21,7.94,1.72,12.03,1.7,1.95,0,3.89-.15,5.83-.41,1.83-.24,4.17-.38,5.71-1.48.63-.45-.04-1.1-.56-1.16-1.72-.19-3.61.51-5.32.75-1.94.27-3.91.41-5.87.44-3.83.06-7.67-.37-11.43-1.06-.87-.16-1.31.93-.39,1.22h0Z" style="fill:#212121;"/><path d="M44.54,92.9c-1.67-.91-3.53-1.43-5.22-2.32s-3.4-2-4.87-3.31c-1.38-1.23-2.59-2.64-3.56-4.22-.49-.79-.91-1.61-1.25-2.47s-.53-1.7-1.2-2.42c-.31-.33-.99-.17-1.12.25-.32,1,.1,1.93.5,2.86.37.87.8,1.72,1.3,2.52,1.07,1.74,2.43,3.31,3.96,4.65s3.32,2.53,5.17,3.47,3.88,1.87,5.94,2.1c.67.07.92-.8.35-1.11h0Z" style="fill:#212121;"/><path d="M43.47,97.33c-2.04-.94-4.46-1.23-6.62-1.86-2.33-.68-4.59-1.57-6.7-2.77-1.77-1.01-3.42-2.31-4.73-3.87-.61-.73-1.16-1.53-1.61-2.37-.43-.79-.66-2-1.55-2.38-.31-.13-.68-.08-.89.2-.6.79.06,1.88.44,2.65.5,1,1.12,1.93,1.85,2.78,1.4,1.65,3.07,3.02,4.93,4.14,2.12,1.28,4.5,2.12,6.88,2.77,1.25.34,2.52.61,3.8.85s2.68.55,4.01.5c.34-.01.58-.46.2-.64h0Z" style="fill:#212121;"/><path d="M8.33,69.31c-1.16,2.57-.94,5.79-.22,8.46s2.08,5.5,4.25,7.44c.54.48,1.69-.07,1.21-.81-1.45-2.24-2.82-4.5-3.55-7.09-.36-1.28-.62-2.56-.75-3.89s-.12-2.69-.24-4.03c-.03-.39-.58-.41-.72-.08h0Z" style="fill:#212121;"/><path d="M62.24,18.87c4.63-.69,9.24-1.62,13.92-1.97,2.19-.17,4.4-.21,6.6-.08,2.34.14,4.64.82,6.97.87.82.02,1.24-1.02.45-1.43-2.05-1.06-4.68-1.18-6.95-1.32s-4.72-.1-7.07.09c-4.85.4-9.61,1.36-14.25,2.81-.78.24-.4,1.13.33,1.02h0Z" style="fill:#212121;"/><path d="M104.51,22.22c1.88,1.96,4.3,3.44,6.41,5.15s4.17,3.45,6.14,5.32,3.91,3.9,5.68,6.01,3.11,4.53,5.05,6.35c.42.39,1.41.06,1.19-.59-.84-2.49-3.01-4.77-4.69-6.75-1.8-2.12-3.72-4.13-5.75-6.03s-4.14-3.72-6.37-5.38-4.6-3.55-7.22-4.57c-.32-.13-.75.18-.44.5h0Z" style="fill:#212121;"/><path d="M143.55,81.48c-.33,2.46-.63,5.18-1.89,7.36-.62,1.06-1.49,1.96-2.53,2.62-1.12.7-2.43,1-3.54,1.71-.66.42-.37,1.26.4,1.28,1.47.04,2.98-.64,4.22-1.4s2.4-1.81,3.16-3.14c1.47-2.57,1.56-5.54,1.5-8.43-.02-.76-1.22-.71-1.31,0h0Z" style="fill:#212121;"/><path d="M36.85,45.35c.44.99,1.85,1.7,2.81,2.11,1.14.5,2.35.84,3.59,1,2.48.32,5.15.06,7.29-1.3.72-.46.21-1.5-.6-1.23-2.05.68-4.17,1.11-6.33.88-1.02-.1-2.06-.35-3.01-.74-.48-.19-.96-.4-1.43-.64-.5-.26-1-.62-1.56-.75-.34-.08-.96.21-.76.65h0Z" style="fill:#212121;"/><path d="M81.95,67.81c6.15.26,12.26.65,18.31,1.86,2.94.59,5.84,1.33,8.71,2.22s5.79,2.29,8.78,2.79c.46.08,1.11-.46.59-.86-2.35-1.81-5.56-2.7-8.35-3.56-3.01-.93-6.08-1.68-9.18-2.26-6.17-1.16-12.57-1.78-18.84-1.18-.72.07-.74.96,0,.99h0Z" style="fill:#212121;"/><path d="M38.09,70.54c1.89-.43,3.78-.84,5.7-1.15s3.83-.49,5.75-.71c.76-.09.55-1.17-.18-1.15-3.88.12-7.87.91-11.56,2.1-.67.22-.36,1.05.29.91h0Z" style="fill:#212121;"/><path d="M109.18,29.76c4,3.6,7.81,7.4,11.4,11.4.3.34.76.48,1.19.27.34-.17.6-.68.31-1.02-3.61-4.22-7.74-7.99-12.29-11.17-.37-.26-1.03.14-.61.52h0Z" style="fill:#212121;"/><path d="M62.88,37.45c2.05-2.31,3.48-5.49,2.61-8.6-.75-2.66-3.15-4.55-5.62-5.53-5.77-2.29-12.33-.68-17.14,3-3.79,2.9-7.44,8.13-5.42,13.07,1.78,4.38,6.69,5.39,10.94,4.92,5.16-.57,10.87-2.84,14.47-6.69.87-.92-.65-2.03-1.41-1.22-2.72,2.9-6.64,4.73-10.45,5.69-3.29.82-7.85,1.31-10.46-1.38-3.21-3.31-1.17-8.25,1.51-11.15,3.38-3.66,8.56-6.01,13.58-5.54,2.82.26,6.06,1.48,7.56,4.04,1.69,2.9.13,6.25-1.88,8.52-.8.9,1.01,1.67,1.73.87Z" style="fill:#212121;"/><path d="M13.85,61.13c-.18,2.06-.03,4.4,1.36,6.05,1.16,1.37,2.92,1.94,4.68,1.88,4.25-.15,8.3-3,10.86-6.24s3.96-7.8,3.66-11.93c-.14-1.91-.66-4-2.17-5.3-1.8-1.53-4.28-1.29-6.35-.52-3.32,1.23-6.05,3.83-8.11,6.65s-3.57,6-3.93,9.42c-.12,1.11,1.88,1.12,2,0,.28-2.64,1.44-5.19,2.87-7.4,1.53-2.37,3.51-4.6,5.93-6.08,1.52-.93,3.83-2.06,5.62-1.21,1.36.64,1.88,2.36,2.07,3.73,1,7.14-3.83,15.24-11,16.96-1.42.34-3.02.38-4.13-.57-1.51-1.29-1.52-3.62-1.36-5.43.1-1.12-1.9-1.12-2,0Z" style="fill:#212121;"/><path d="M132.91,72.86c1.61,1.24,3.62,2.27,5.68,1.58s2.73-2.74,2.75-4.74c.04-3.84-1.12-8.29-3.71-11.21-1.8-2.02-4.76-4.07-7.61-3.28-3.34.92-4.16,4.95-3.52,7.91.81,3.76,3.45,7.37,6.42,9.74.86.69,2.41-.42,1.41-1.22-2.16-1.72-3.99-4.07-5.11-6.6-1.02-2.3-1.67-5.63.39-7.6,1.57-1.51,3.94-.15,5.3.98,2.32,1.94,3.47,4.77,4.08,7.68.35,1.7.78,4.08-.01,5.73-.99,2.05-3.36.8-4.64-.18-.87-.67-2.42.44-1.41,1.22Z" style="fill:#212121;"/></g><path d="M99.57,75.13c6.81,23.39,12.53,55.31,5.4,72.82-7.85,19.28-29.25,24.75-46.83,17.58-10.2-4.16-17.86-13.74-19.96-25.4-3.86-21.48,15.96-66.85,15.96-66.85,23.94-4.17,45.43,1.86,45.43,1.86Z" style="fill:#fcddd8;"/><path d="M98.73,75.33c4.19,18.59,8.87,37.67,7.63,56.88-.52,7.99-2.33,15.91-7.23,22.41-4.59,6.09-11.4,10.06-18.84,11.52s-15.37.45-22.19-2.7c-7.86-3.63-13.71-10.73-16.34-18.92-2.06-6.38-1.88-13.02-.93-19.85,1-7.17,2.84-14.23,4.9-21.16,2.95-9.91,6.47-19.69,10.52-29.21.07-.16.14-.32.21-.48l-1.67,1.43c13.6-2.36,27.83-1.8,41.32,1.09.96.21,1.93.41,2.87.68,2.67.75,4.06-2.99,1.21-3.78-6.46-1.8-13.26-2.65-19.93-3.14s-13.14-.44-19.67.24c-1.72.18-3.44.39-5.14.65-1.46.22-2.91.27-3.6,1.77-.16.34-.3.69-.45,1.04-5.71,13.44-10.62,27.41-13.75,41.69-1.77,8.09-3.25,16.73-1.82,24.98,1.9,10.99,9.26,20.92,19.3,25.8,8.87,4.31,19.66,5.32,29.14,2.54s17.58-9.59,21.8-18.79c4.36-9.49,4.36-20.73,3.59-30.94-.92-12.21-3.28-24.35-6.61-36.12-.77-2.72-1.67-5.37-2.62-8.03-.32-.9-1.91-.61-1.69.4Z" style="fill:#0d0d28;"/><path d="M42.97,99.62c-7.18-1.55-14.28-3.5-21.23-5.86-2.71-.92-5.46-1.82-7.85-3.44-1.94-1.32-3.66-2.97-5.04-4.86-2.9-3.96-4.32-8.81-4.01-13.71s2.73-9.71,5.01-14.04c9.33-17.73,24.67-32.52,43.23-40.29,8.75-3.66,18.22-5.59,27.7-5.23s19.51,3.4,27.98,8.52c17.73,10.72,30.28,29.57,36.48,49.07,2.41,7.59,4.5,16.92-1.59,23.3-2.69,2.82-6.25,4.57-9.83,5.6-5.36,1.55-11.05,2.31-16.56,3.12-3.29.48-6.59.88-9.89,1.2-1.14.11-2.4.74-2.4,2.06,0,1.17,1.26,2.18,2.4,2.06,6.99-.68,13.95-1.67,20.85-2.98,4.72-.89,9.43-1.86,13.66-4.26s7.44-5.69,9.22-10.09c2-4.95,1.55-10.42.34-15.5-2.05-8.58-5.53-16.87-9.81-24.57-5.88-10.58-13.72-20.18-23.27-27.65-8.67-6.77-18.89-11.56-29.8-13.27-10.3-1.62-20.89-.68-30.83,2.45-20.42,6.42-37.96,21.06-49.1,39.21-2.79,4.55-5.35,9.42-7.19,14.44s-1.91,10.93-.13,16.1,5.33,9.71,9.95,12.74c2.7,1.77,5.7,2.79,8.74,3.84,3.69,1.26,7.41,2.41,11.17,3.44,3.49.96,7,1.81,10.53,2.58,1.14.25,2.57-.24,2.95-1.44s-.55-2.3-1.68-2.54h0Z" style="fill:#0d0d28;"/><path d="M127.23,102.2c4.82-4.17,5.89-11.12,1.79-16.17-2.3-2.83-5.43-4.87-8.66-6.5-4.4-2.21-9.16-3.73-13.93-4.94-6.17-1.56-12.47-2.57-18.78-3.31-7.28-.85-14.62-1.38-21.95-1.29-6.9.08-13.81.7-20.6,1.95-5.54,1.02-11.08,2.48-16.2,4.87-3.97,1.85-8.3,4.45-10.43,8.44-2.01,3.78-1.58,8.37,1.47,11.45,1.02,1.02,3.57.13,2.98-1.49-.3-.82-.62-1.62-.93-2.44-.27-.72-.48-1.52-.52-2-.15-1.52-.04-2.42.57-3.83.46-1.07,1.5-2.39,2.48-3.32,1.41-1.33,3.01-2.42,4.72-3.33,9.78-5.24,21.6-6.74,32.53-7.17,13.08-.52,26.49.7,39.3,3.38,4.81,1.01,9.62,2.26,14.18,4.12,3.24,1.32,6.46,2.92,9.1,5.24,2.05,1.8,3.71,4.17,3.9,6.71.22,3.05-1.36,5.72-2.84,8.43-.61,1.13.96,1.94,1.82,1.21h0Z" style="fill:#0d0d28;"/><path class="eye" d="M62.85,116.97c-.68,3.62-3.53,6.12-6.37,5.58-2.84-.53-4.58-3.9-3.9-7.51.68-3.62,3.53-6.12,6.37-5.58,2.84.53,4.58,3.9,3.9,7.51Z" style="fill:#323333;"/><path class="eye" d="M84.93,118.98c-.04,3.68,2.26,6.69,5.15,6.72,2.89.03,5.26-2.92,5.3-6.6.04-3.68-2.26-6.69-5.15-6.72-2.89-.03-5.26,2.92-5.3,6.6Z" style="fill:#323333;"/><g style="opacity:.13;"><path d="M48.2,125.8c1.84,0,3.85-1.31,3.85-3.31s-2.01-3.31-3.85-3.31-3.85,1.31-3.85,3.31,2.01,3.31,3.85,3.31h0Z" style="fill:#17152b;"/><path d="M98.99,130.57c1.84,0,3.85-1.31,3.85-3.31s-2.01-3.31-3.85-3.31-3.85,1.31-3.85,3.31,2.01,3.31,3.85,3.31h0Z" style="fill:#17152b;"/></g><path class="mouth" d="M70.78,116.98c-1-.06-1.97.39-2.59,1.17-.55.69-.57,1.54-.58,2.38-.03,1.48.06,2.99.41,4.44.31,1.28.92,2.57,1.95,3.42,1.2.99,2.82,1.32,4.33,1.02,3-.6,4.33-3.83,4.94-6.49.37-1.61.9-3.52-.64-4.69-.78-.6-1.71-.64-2.65-.73l-5.17-.52c-.72-.07-1.5.56-1.5,1.29,0,.84.79,1.22,1.5,1.29l3.66.37,2.06.21c.19.02.56.08.06,0,.08.01.16-.02.07.01-.08.02.02.07,0,.01-.05-.17.04.11.04.06,0,.03-.11-.2-.04-.02.06.14,0,0,0-.03.02.06.05.12.06.18.02.16,0-.01,0-.03,0,.11-.01.22-.03.34,0,.06,0,.06-.04.23-.02.14-.05.29-.08.43-.06.33-.13.67-.21,1-.14.6-.3,1.19-.51,1.77-.1.27-.2.55-.32.81-.02.05-.17.34-.07.16-.08.15-.16.3-.24.44-.12.21-.26.41-.41.6-.06.08-.23.26-.06.09-.1.11-.21.21-.32.31-.08.07-.18.13-.26.2.18-.15.06-.04-.06.03-.11.06-.23.1-.34.16-.14.07.09-.04.1-.04-.07.01-.14.04-.21.06-.16.04-.31.04-.05.03-.14,0-.28.02-.42.01-.07,0-.42-.05-.23-.01s-.19-.05-.26-.06c-.07-.02-.14-.04-.21-.06.02,0,.23.11.1.03-.09-.06-.2-.09-.29-.14-.05-.02-.09-.05-.14-.08.02.01.19.16.09.05-.09-.1-.22-.18-.32-.27-.04-.04-.07-.07-.11-.11.01.01.15.18.06.06-.06-.09-.13-.17-.2-.26-.07-.11-.14-.21-.21-.33-.03-.05-.05-.1-.08-.15-.1-.17.08.19,0,0-.25-.57-.42-1.16-.54-1.77-.01-.06-.08-.44-.05-.24-.02-.16-.04-.31-.06-.47-.04-.34-.07-.69-.08-1.04-.03-.71-.03-1.42,0-2.13,0-.07,0-.14,0-.21.01-.22-.05.15,0-.05.02-.06.03-.13.06-.18-.08.16,0,0,.04-.02,0,0-.16.15-.02.04.08-.06.06-.06-.05.02l.11-.06s-.24.08-.06.03c.2-.05-.32,0,.06.02.72.04,1.5-.54,1.5-1.29,0-.81-.78-1.25-1.5-1.29Z" style="fill:#323333;"/><path class="tounge" d="M76.41,124.38c-1.85-1-4.31-1.24-6.27-.4,0,0,.45,4.63,2.69,4.24,2.25-.39,3.58-3.84,3.58-3.84Z" style="fill:#323333;"/><g id="c"><path d="M154.52,29.16c-4.99,1.15-6.39,2.56-7.55,7.55-.11.47-.77.47-.88,0-1.15-4.99-2.56-6.39-7.55-7.55-.47-.11-.47-.77,0-.88,4.99-1.15,6.39-2.56,7.55-7.55.11-.47.77-.47.88,0,1.15,4.99,2.56,6.4,7.55,7.55.47.11.47.77,0,.88Z" style="fill:#ffd5ab;"/></g><g id="d"><path d="M26.28,117.37c-4.82,1.12-6.18,2.48-7.3,7.3-.1.45-.75.45-.85,0-1.12-4.82-2.48-6.18-7.3-7.3-.45-.1-.45-.75,0-.85,4.82-1.12,6.18-2.48,7.3-7.3.1-.45.75-.45.85,0,1.12,4.82,2.48,6.18,7.3,7.3.45.1.45.75,0,.85Z" style="fill:#cad5eb;"/></g><g id="e"><path d="M139,10.45c-5.94,1.37-7.61,3.05-8.99,8.99-.13.56-.92.56-1.05,0-1.37-5.94-3.05-7.61-8.99-8.99-.56-.13-.56-.92,0-1.05,5.94-1.37,7.61-3.05,8.99-8.99.13-.56.92-.56,1.05,0,1.37,5.94,3.05,7.61,8.99,8.99.56.13.56.92,0,1.05Z" style="fill:#ee5e5e;"/></g></svg>
                </div>
            </div>
            <div class="controls">
                <div class="input-group">
                    <label for="color">Color</label>
                    <input type="color" id="color" value="#e66465" />
                </div>
                <div class="input-group">
                    <label for="brightness">Brightness</label>
                    <input type="range" id="brightness" min="0" max="255" value="0" step="1" />
                </div>
                <div class="input-group">
                    <label for="effect">Effect</label>
                    <select id="effect">
                        <option value="solid">Solid color</option>
                        <option value="breathe">Breathe</option>
                        <option value="rainbow">Rainbow</option>
                        <option value="blink">Blink</option>
                    </select>
                </div>
            </div>

            <div class="loader">Connecting...</div>

            <script>
                window.config = {
                    power: 'off',
                    color: '#FF0000',
                    brightness: '255',
                    effect: 'solid',
                }

                window.desiredConfig = {
                    ...window.config
                }

                function configToString(config) {
                    return `config|${config.power}|${config.color}|${config.brightness}|${config.effect}`;
                }

                function configFromString(configString) {
                    const values = configString.split('|').slice(1);
                    return {
                        power: values[0],
                        color: values[1],
                        brightness: values[2],
                        effect: values[3],
                    }
                }

                function applyConfigToUI(config) {
                    document.getElementById('b').style.fill = config.color;
                    document.getElementById('color').value = config.color;
                    config.power === 'on' ? document.querySelector('.mushroom').classList.remove('off') : document.querySelector('.mushroom').classList.add('off');
                    document.getElementById('brightness').value = config.brightness;
                    document.getElementById('effect').value = config.effect;
                }

                function connectToWebsocket() {
                    if (window.ws && window.ws.readyState === window.ws.OPEN)
                        window.ws.close();

                    console.log("Creating websocket");
                    const hostname = window.location.host;
                    window.ws = new WebSocket(`ws://${hostname}/ws`);
                    window.ws.addEventListener("error", (event) => {
                        console.log(event);
                        window.ws.close();
                        document.querySelector('.loader').textContent = `Something went wrong :(`
                    });
                    window.ws.addEventListener("close", (event) => {
                        document.querySelector('.loader').classList.remove('hidden');

                        console.log("Websocket closed, reconnecting in 2 seconds");
                        setTimeout(connectToWebsocket, 2000);
                    });
                    window.ws.addEventListener("open", (event) => {
                        document.querySelector('.loader').classList.add('hidden');
                    });
                    window.ws.addEventListener("message", (e) => {
                        console.log(`text: ${e.data}`);
                        if(e.data.startsWith('config')) {
                            window.config = configFromString(e.data);
                            window.desiredConfig = configFromString(e.data);
                            applyConfigToUI(window.config);
                        }
                    });
                }
                connectToWebsocket();

                setInterval(() => {
                    if (window.ws && window.ws.readyState === window.ws.OPEN)
                        window.ws.send('ping');
                }, 5000);

                function sendDesiredConfig() {
                    if (window.ws && window.ws.readyState === window.ws.OPEN)
                        window.ws.send(configToString(window.desiredConfig));
                }

                const mushroomEl = document.querySelector('.mushroom');
                mushroomEl.addEventListener('click', (e) => {
                    mushroomEl.classList.add('squish');
                    
                    window.desiredConfig.power = mushroomEl.classList.contains('off') ? 'on' : 'off';
                    sendDesiredConfig();

                    setTimeout(() => mushroomEl.classList.remove('squish'), 650);
                });

                document.getElementById('color').addEventListener('change', (e) => {
                    if(window.desiredConfig.color === e.target.value) return;
                    window.desiredConfig.color = e.target.value;
                    sendDesiredConfig();
                });

                document.getElementById('brightness').addEventListener('change', (e) => {
                    if(window.desiredConfig.brightness === e.target.value) return;
                    window.desiredConfig.brightness = e.target.value;
                    sendDesiredConfig();
                });

                document.getElementById('effect').addEventListener('change', (e) => {
                    if(window.desiredConfig.effect === e.target.value) return;
                    window.desiredConfig.effect = e.target.value;
                    sendDesiredConfig();
                });

                applyConfigToUI(window.config);
            </script>
        </body>
        </html>
    )";

    static const size_t htmlContentLength = strlen_P(htmlContent);

    static const char *manifestContent PROGMEM = R"(
      {
        "name": "Mushroom lamp",
        "short_name": "Mushroom",
        "theme_color": "#211e33",
        "background_color": "#211e33",
        "display": "fullscreen",
        "orientation": "portrait",
        "scope": "/",
        "start_url": "/"
      }
    )";
    static const size_t manifestContentLength = strlen_P(manifestContent);

    void init()
    {
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "text/html", (uint8_t *)htmlContent, htmlContentLength);
        });

        server.on("/app.webmanifest", HTTP_GET, [](AsyncWebServerRequest *request) {
          request->send(200, "application/json", (uint8_t *)manifestContent, manifestContentLength);
      });

        server.onNotFound([](AsyncWebServerRequest *request) {
            request->send(404, "text/plain", "Not found");
        });

        ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
            (void)len;
            
            if (type == WS_EVT_CONNECT) {
              client->setCloseClientOnQueueFull(false);
              client->ping();
              client->text(Config::toString());
            } else if (type == WS_EVT_ERROR) {
              Serial.println("ws error");
            } else if (type == WS_EVT_DATA) {
              AwsFrameInfo *info = (AwsFrameInfo *)arg;
              String msg = "";
              if (info->final && info->index == 0 && info->len == len) {
                if (info->opcode == WS_TEXT) {
                  data[len] = 0;
                  char* stringData = (char *)data;

                  if(startsWith(stringData, "ping")) return;

                  Serial.printf("ws text received: %s\n", stringData);

                  if(startsWith(stringData, "config")) {
                    Config::fromString(stringData);
                    ws.textAll(Config::toString());
                  }
                }
              }
            } 
        });

        server.addHandler(&ws).addMiddleware([](AsyncWebServerRequest *request, ArMiddlewareNext next) {
          if (ws.count() >= 3) {
            Serial.println("Rejecting client, maximum count reached.");
            request->send(503, "text/plain", "Server is busy");
          } else {
            next();
          }
        });
      
        server.addHandler(&ws);

        server.begin();
    }

    void loop()
    {
      if (millis() - lastHeap >= 2000) {
        lastHeap = millis();
        ws.cleanupClients();
      }
    }

}