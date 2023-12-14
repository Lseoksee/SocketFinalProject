package com.seok;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

import javax.swing.JOptionPane;

public class ConnectServer extends Client implements ActionListener, KeyListener {
    public static final int BUFFER_SIZE = 512;
    public static final int PORT = 9000;

    String userId;
    byte[] message = new byte[BUFFER_SIZE];
    DatagramSocket socket;
    DatagramPacket sendpacket;
    DatagramPacket recvPacket;

    private void Connect(String ipAddr) throws SocketException, UnknownHostException, IOException {
        socket = new DatagramSocket();
        socket.setSoTimeout(3000);

        byte sendByte[] = new byte[BUFFER_SIZE];
        InetAddress addr = InetAddress.getByName(ipAddr);

        sendpacket = new DatagramPacket(sendByte, sendByte.length, addr, PORT);
        recvPacket = new DatagramPacket(sendByte, sendByte.length, addr, PORT);

        message = "connect".getBytes();
        sendpacket.setData(message);
        socket.send(sendpacket);

        socket.receive(recvPacket);
        socket.setSoTimeout(0);

        userId = new String(recvPacket.getData(), "euc-kr").trim().split("-")[1];
        playerList.addElement(userId + " (나)");
        socket.setSoTimeout(0);
        new Thread(() -> recvThread()).start();
    }

    private void recvThread() {
        try {
            while (true) {
                recvPacket.setData(new byte[BUFFER_SIZE]);
                socket.receive(recvPacket);
                String parseData[] = new String(recvPacket.getData(), "euc-kr").trim().split("-");
                switch (parseData[0]) {
                    case "connect":
                        playerList.addElement(parseData[1]);
                        break;
                    case "next":
                        spelling.setText(parseData[1]);
                        if (parseData[2].equals(userId)) {
                            state.setText("이번순서: 나");
                            state.setForeground(Color.green);
                            inputMessege.setEditable(true);
                        } else {
                            state.setText("이번순서: " + parseData[2]);
                            state.setForeground(Color.black);
                            inputMessege.setEditable(false);
                        }

                        break;
                    case "user":
                        String parseUsers[] = parseData[1].split(",");
                        for (String userId : parseUsers) {
                            playerList.addElement(userId);
                        }
                        break;
                    case "exit":
                        playerList.removeElement(parseData[1]);
                        break;
                    default:
                        break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void pushMesage(String str) throws IOException {
        sendpacket.setData(str.getBytes("euc-kr"));
        socket.send(sendpacket);
    }

    public void exitUser() {
        try {
            pushMesage("exit-" + userId);
        } catch (Exception e) {
        }
    }

    private void checkLastSpell() throws IOException {
        if (inputMessege.getText().isEmpty()) {
            return;
        }

        String inputData = inputMessege.getText();
        String viewData = spelling.getText();

        if (viewData.charAt(viewData.length() - 1) == inputData.charAt(0)) {
            pushMesage("spelling-" + inputData);
        } else {
            state.setText("올바른 끝말을 입력하세요!");
            state.setForeground(Color.RED);
        }

        inputMessege.setText(null);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        try {
            if (e.getSource() == connetAddr && playerList.isEmpty()) {
                Connect(ipAddrField.getText());
                spelling.setText("이번순서가 입력할때 까지 대기...");
            } else if (e.getSource() == inputButton) {
                checkLastSpell();
            }
        } catch (SocketTimeoutException ex) {
            JOptionPane.showMessageDialog(frame, "서버에 연결할 수 없습니다.", "오류", JOptionPane.ERROR_MESSAGE);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public void keyTyped(KeyEvent e) {
    }

    @Override
    public void keyPressed(KeyEvent e) {
        try {
            if (e.getSource() == inputMessege && e.getKeyCode() == KeyEvent.VK_ENTER) {
                checkLastSpell();
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }

    }

    @Override
    public void keyReleased(KeyEvent e) {
    }
}