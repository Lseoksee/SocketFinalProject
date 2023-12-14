package com.seok;

import java.awt.Font;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JTextField;
import javax.swing.UIManager;

public class Client {
    static JFrame frame;
    static JTextField inputMessege;
    static JButton inputButton;
    static JList<String> listView;
    static DefaultListModel<String> playerList;
    static JLabel spelling;
    static JLabel state;
    static JTextField ipAddrField;
    static JButton connetAddr;

    public static void main(String[] args) {
        // os 스타일 ui로 변경
        try {
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (Exception e) {
        }

        frame = new JFrame("끝말잇기 하실래요?");
        frame.setSize(700, 500); // (프레임크기-객체크기)*
        frame.setResizable(false);
        frame.setFocusable(true);
        frame.setLayout(null);
        new Client().startApp();
    }

    public void startApp() {
        int frameWidth = frame.getWidth() - 16;
        int frameHeight = frame.getHeight() - 16;
        ConnectServer server = new ConnectServer();

        inputMessege = new JTextField();
        inputMessege.setFont(new Font("맑은 고딕", Font.PLAIN, 15));
        inputMessege.setEditable(false);
        inputMessege.addKeyListener(server);
        inputMessege.setBounds(5, frameHeight - 60, frameWidth - 100, 30);

        inputButton = new JButton("입력");
        inputButton.addActionListener(server);
        inputButton.setBounds(frameWidth - 85, frameHeight - 60, 80, 30);

        playerList = new DefaultListModel<>();

        JLabel palayerListLabel = new JLabel("플레이어 목록", JLabel.CENTER);
        palayerListLabel.setFont(new Font("맑은 고딕", Font.PLAIN, 15));
        palayerListLabel.setBounds(frameWidth - 120, 10, 120, 20);

        listView = new JList<>(playerList);
        listView.setFont(new Font("맑은 고딕", Font.PLAIN, 15));
        listView.setBounds(frameWidth - 120, 40, 120, frameHeight - 110);

        state = new JLabel();
        state.setHorizontalAlignment(JLabel.CENTER);
        state.setOpaque(true);
        state.setFont(new Font("맑은 고딕", Font.PLAIN, 17));
        state.setBounds(0, 60, frameWidth - listView.getWidth(), 20);

        spelling = new JLabel("연결하기를 눌러 게임시작", JLabel.CENTER);
        spelling.setFont(new Font("맑은 고딕", Font.PLAIN, 30));
        spelling.setBounds(0, frameHeight / 2 - 40, frameWidth - listView.getWidth(), 30);

        JLabel ipAddrLable = new JLabel("연결할 ip주소:", JLabel.RIGHT);
        ipAddrLable.setFont(new Font("맑은 고딕", Font.PLAIN, 15));
        ipAddrLable.setBounds(0, 10, frameWidth / 2 - 100, 25);

        ipAddrField = new JTextField("127.0.0.1");
        ipAddrField.setFont(new Font("맑은 고딕", Font.PLAIN, 14));
        ipAddrField.setBounds(ipAddrLable.getWidth() + 5, 10, 100, 25);

        connetAddr = new JButton("연결하기");
        connetAddr.setFont(new Font("맑은 고딕", Font.PLAIN, 14));
        connetAddr.setBounds(ipAddrField.getX() + ipAddrField.getWidth() + 5, 10, 100, 25);
        connetAddr.addActionListener(server);

        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                server.exitUser();
                e.getWindow().setVisible(false);
                e.getWindow().dispose();
                System.exit(0);
            }
        });

        frame.add(inputMessege);
        frame.add(inputButton);
        frame.add(palayerListLabel);
        frame.add(listView);
        frame.add(spelling);
        frame.add(ipAddrField);
        frame.add(ipAddrLable);
        frame.add(connetAddr);
        frame.add(state);

        frame.setVisible(true);
    }
}
