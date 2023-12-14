package com.seok;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) {
        try {
            DatagramSocket socket = new DatagramSocket();
            byte sendByte[] = new byte[512];
            InetAddress addr = InetAddress.getByName("127.0.0.1");
            DatagramPacket sendpacket = new DatagramPacket(sendByte, sendByte.length, addr, 9000);
            DatagramPacket recvPacket = new DatagramPacket(sendByte, sendByte.length, addr, 9000);

            String message = "connect";
            sendpacket.setData(message.getBytes("euc-kr"));
            socket.send(sendpacket);
            Scanner sc = new Scanner(System.in);

            new Thread(() -> {
                while (true) {
                    try {
                        recvPacket.setData(new byte[512]);
                        socket.receive(recvPacket);
                        String data = new String(recvPacket.getData(), "euc-kr");
                        System.out.println(data);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

            }).start();

            while (true) {
                String line = sc.nextLine();
                if (line.isEmpty()) {
                    break;
                }

                sendpacket.setData(line.getBytes("euc-kr"));
                socket.send(sendpacket);
            }

            socket.disconnect();
            socket.close();
            sc.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
