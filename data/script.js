// Complete project details: https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
}

function getValues() {
    websocket.send("getValues");
}


function display_c() {
    var refresh = 1000; // Refresh rate in milli seconds
    mytime = setTimeout('display_ct()', refresh)
}

function display_ct() {
    var x = new Date()
    var x1=x.getMonth() + 1+ "/" + x.getDate() + "/" + x.getFullYear(); 
    x1 = x1 + " - " +  x.getHours( )+ ":" +  x.getMinutes() + ":" +  x.getSeconds();
    document.getElementById('currentTime').innerHTML = x1;
    display_c();
}

function getCurrentTime() {
    var currentdate = new Date();
    var t = currentdate.getHours() + ":" + currentdate.getMinutes() + ":" + currentdate.getSeconds();
    console.log(t);

    let e = { "command": "settime", "H": currentdate.getHours(), "M": currentdate.getMinutes(), "s": currentdate.getSeconds() };
    console.log(JSON.stringify(e));
    websocket.send(JSON.stringify(e));

}

function logout() {
    console.log("Logout");
}
function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getValues();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function updateSliderPWM(element) {
    var sliderNumber = element.id.charAt(element.id.length - 1);
    var sliderValue = document.getElementById(element.id).value;
    document.getElementById("sliderValue" + sliderNumber).innerHTML = sliderValue;
    console.log(sliderValue);
    websocket.send(sliderNumber + "s" + sliderValue.toString());
}

function toggleCheckbox(element) {
    var switchId = element.id;
    var switchValue = "0";
    if (element.checked) { switchValue = "1"; }
    console.log(switchId + "s" + switchValue);
    websocket.send(switchId + "s" + switchValue);
}



function save() {
    let e = { "command": "save" };
    console.log(JSON.stringify(e));
    websocket.send(JSON.stringify(e));
}

function timeSelected(element) {
    console.log(element.value);
    let e = { "time": element.id, "value": element.value };
    console.log()
    console.log(JSON.stringify(e));
    websocket.send(JSON.stringify(e));
}


function checkboxClicked(element) {

    var checkboxId = element.id;
    console.log(element.id);
    var timeElementId = checkboxId.substring(8) + "value";
    console.log(timeElementId);
    document.getElementById(timeElementId).disabled = !element.checked;
    let e = { "checkbox": element.id, "value": element.checked };


    console.log(JSON.stringify(e));
    websocket.send(JSON.stringify(e));

}





function onMessage(event) {

    console.log(event.data);
    console.log("message received");
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);


    for (var i = 0; i < keys.length; i++) {
        var key = keys[i];
        var value = myObj[key];
        if (value == "0") {
            document.getElementById("relay" + (i + 1).toString()).checked = false;
        }
        else {
            document.getElementById("relay" + (i + 1).toString()).checked = true;
        }
        console.log("Relay" + (i + 1));
        console.log(myObj[key]); //value
        console.log(key);

    }




}
