var express = require("express");
var app = express();
var http = require('http');
var childProcess = require('child_process');
var ws = require('ws');

var createIP = require('./createIP');
var ipc = require('node-ipc');

var static_port = 8080;
var stream_port = 8082;
var socket_port = 8084;

var development = true;
if(!development) {
    createIP();
}

/// VIDEO STREAMING
var STREAM_MAGIC_BYTES = 'jsmp'; // Must be 4 bytes
var width = 320;
var height = 240;

// WEBSOCKET SERVER
var wsServer = new(ws.Server)({ port: socket_port });
console.log('WebSocket server listening on port ' + socket_port);


var commands = [];

var process_executing = false;
function execute_next_item () {
    if (!process_executing && commands.length){
        process_executing = true;
        commands.splice(0,1);
        process_executing = false;
        execute_next_item();
    }
};

/*
*    TCP/IP SOCKET – CLIENT & SERVER
*/
wsServer.on('connection', function(socket) {
    var streamHeader = new Buffer(8);
    streamHeader.write(STREAM_MAGIC_BYTES);
    streamHeader.writeUInt16BE(width, 4);
    streamHeader.writeUInt16BE(height, 6);
    socket.send(streamHeader, { binary: true });

    console.log('New WebSocket Connection (' + wsServer.clients.length + ' total)');

    socket.on("message", function(code) {
        console.log("RECEIVED: ", code);
        commands.push(code);
        execute_next_item(code);
    })

    socket.on('close', function(code, message) {
        console.log('Disconnected WebSocket (' + wsServer.clients.length + ' total)');
    });
});

wsServer.broadcast = function(data, opts) {
    for (var i in this.clients) {
        if (this.clients[i].readyState == 1) {
            this.clients[i].send(data, opts);
        } else {
            console.log('Error: Client (' + i + ') not connected.');
        }
    }
};

/*
*    UNIX SOCKET – SERVER & C++ PROGRAM
*/
ipc.config.appspace = "breakerbot.";
ipc.config.id = 'socket';
ipc.config.retry= 1500;
ipc.config.rawBuffer=true;
ipc.config.encoding='hex';

ipc.serve(function() {
    ipc.server.on('connect', function(socket){
        ipc.server.emit(
            socket,
            [0xaa]
        );
    });
    ipc.server.on('data', function(data,socket){
        console.log("Data",data);
        ipc.log('got a message', data.toString('utf-8'));
        ipc.server.emit(
            socket,
            [0x77,0x66]
        );
    });
});

ipc.server.start();

// HTTP STATIC SERVER
app.set('port', static_port);
app.use(express.static("./client"));
http.createServer(app).listen(app.get('port'), function() {
    console.log('HTTP server listening on port ' + app.get('port'));
});

// HTTP SERVER FOR MPEG1 STREAM
http.createServer(function(req, res) {
    console.log('Stream Connected: ' + req.socket.remoteAddress + ':' + req.socket.remotePort + ' size: ' + width + 'x' + height);
    req.on('data', function(data) {
        wsServer.broadcast(data, { binary: true });
    });
}).listen(stream_port, function() {
    console.log('Listening for video stream on port ' + stream_port);
    childProcess.exec('./bin/do_ffmpeg.sh');
});
