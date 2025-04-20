#!/usr/bin/env python3

# Simple fastAPI endpoint that binds fronendend and an
# openFrameworks app via websockets

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware
import asyncio

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

openframeworks_socket: WebSocket | None = None

clients: list[WebSocket] = []

@app.websocket("/client-ws")
async def websocket_client(websocket: WebSocket):
    await websocket.accept()
    clients.append(websocket)
    print("Frontend client connected")
    try:
        while True:
            data = await websocket.receive_text()
            # print("Got data from frontend:", data)

            # Broadcast to all clients except the sender
            for client in clients:
                if client != websocket:
                    try:
                        await client.send_text(data)
                    except:
                        pass  # Skip dead clients or errors


            # Forward to oF app if connected
            if openframeworks_socket:
                await openframeworks_socket.send_text(data)

    except WebSocketDisconnect:
        print("Client disconnected ", websocket.client)
        clients.remove(websocket)

@app.websocket("/of-ws")
async def websocket_of(websocket: WebSocket):
    global openframeworks_socket
    await websocket.accept()
    openframeworks_socket = websocket
    print("openFrameworks Web Socket connected")

    try:
        while True:
            await asyncio.sleep(1)
    except WebSocketDisconnect:
        print("openFrameworks disconnected")
        openframeworks_socket = None

