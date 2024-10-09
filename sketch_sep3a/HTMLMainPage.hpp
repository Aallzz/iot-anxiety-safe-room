#pragma once

#include <string_view>

struct HTMLMainPage {
  static constexpr std::string_view HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>WebSocket Example</title>
    <style>
        .column {
            float: left;
            width: 30%;
            padding: 10px;
        }
        .row:after {
            content: "";
            display: table;
            clear: both;
        }
    </style>
    <script>
        var gateway = `ws://${window.location.hostname}/ws`;
        var websocket;
        var eventMessages = {};  // Object to store arrays of messages per event type
        const MAX_MESSAGES = 10;  // Maximum number of messages to store per event

        window.addEventListener('load', onLoad);

        function onLoad() {
            initWebSocket();
        }

        function initWebSocket() {
            console.log('Trying to open a WebSocket connection...');
            websocket = new WebSocket(gateway);
            websocket.onopen    = onOpen;
            websocket.onclose   = onClose;
            websocket.onmessage = onMessage;
        }

        function onOpen(event) {
            console.log('Connection opened');
        }

        function onClose(event) {
            console.log('Connection closed');
            setTimeout(initWebSocket, 2000);  // Try to reconnect every 2 seconds
        }

        function onMessage(event) {
            var data = JSON.parse(event.data);
            var eventType = data.event;  // Get the event type

            // If this event type hasn't been seen before, create a new column
            if (!eventMessages[eventType]) {
                eventMessages[eventType] = [];
                createNewEventColumn(eventType);
            }

            // Add the new message to the list for this event
            updateMessagesForEvent(eventType, data);
        }

        function createNewEventColumn(eventType) {
            var row = document.getElementById('eventRows');
            var column = document.createElement('div');
            column.className = 'column';
            column.id = `column-${eventType}`;
            column.innerHTML = `<h2>${eventType}</h2><ul id="messages-${eventType}"></ul>`;
            row.appendChild(column);
        }

        function updateMessagesForEvent(eventType, data) {
            // Add the new message to the beginning of the array for the event
            eventMessages[eventType].unshift(JSON.stringify(data));

            // Limit the array to the last 10 messages
            if (eventMessages[eventType].length > MAX_MESSAGES) {
                eventMessages[eventType].pop();
            }

            // Update the display for this event
            var messageList = document.getElementById(`messages-${eventType}`);
            messageList.innerHTML = "";  // Clear the list

            for (var i = 0; i < eventMessages[eventType].length; i++) {
                var li = document.createElement('li');
                li.textContent = eventMessages[eventType][i];
                messageList.appendChild(li);
            }
        }
    </script>
</head>
<body>
    <h1>ESP WebSocket Room data</h1>
    <div class="row" id="eventRows"></div>  <!-- Container for dynamic event columns -->
</body>
</html>
)rawliteral";
};
