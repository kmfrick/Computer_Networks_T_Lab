
import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

public class DiscoveryServer {

    private static int INVALID_ARGUMENTS = 1;
    private static int INVALID_PORT_NUMBER = 2;
    private static int REPEATED_ARGUMENTS = 3;
    private static int COMMUNICATION_ERROR = 4;
    private static boolean[] socketIsValid;

    private static String IS_A_FILENAME = "^[\\w\\-. ]+$";
    private static String IS_A_POSITIVE_INTEGER = "^[1-9][0-9]*$";

    public static void main(String[] args) {
        int i = 0, j, dsPort, port = -1;

        // Controllo dei parametri in input
        if (args.length < 3 || !args[i].matches(IS_A_POSITIVE_INTEGER)) {
            System.out.println(
                    "DS: Invalid arguments. \nDS: Requested format: DiscoveryServer portNumber {nomefile associated portS}");
            System.exit(INVALID_ARGUMENTS);
        }
        dsPort = Integer.parseInt(args[i]);
        try {
            DatagramSocket clientSocket = new DatagramSocket(dsPort);
            clientSocket.close();
        } catch (SocketException e) {
            System.out.println("DS: Error, port " + dsPort + " is not available");
            System.exit(INVALID_PORT_NUMBER);
        }

        i = 1;
        while (i < args.length) {
            if (!args[i].matches(IS_A_FILENAME) || !args[i + 1].matches(IS_A_POSITIVE_INTEGER)) {
                System.out.println(
                        "DS: Invalid arguments. \nDS: Requested format: DiscoveryServer portNumber [nomefile associatedPort]+"
                                + "DS: i nomefile devono essere tutti distinti tra loro e anche tutte le porte");
                System.exit(INVALID_ARGUMENTS);
            }
            j = i - 2;
            while (j > 0) {
                if (args[j].equals(args[i]) || args[j + 1].equals(args[i + 1])) {
                    System.out.println(
                            "DS: Invalid arguments. \nDS: Requested format: DiscoveryServer portNumber [nomefile associatedPort]+\n"
                                    + "DS: i nomefile devono essere tutti distinti tra loro e anche tutte le porte");
                    System.exit(REPEATED_ARGUMENTS);
                }
                j -= 2;
            }
            i += 2;
        }

        // Creazione e avvio dei rowSwapServer disponibili
        i = 1;
        socketIsValid = new boolean[args.length / 2];
        while (i < args.length) {
            try {
                socketIsValid[(i - 1) / 2] = false;
                port = Integer.parseInt(args[i + 1]);
                RowSwapServer rowSwapServer = new RowSwapServer(args[i], port);
                rowSwapServer.start();
                System.out.println("DS: File " + args[i] + " correttamente connesso alla porta " + port);
                socketIsValid[(i - 1) / 2] = true;
            } catch (SocketException sE) {
                System.out.println(
                        "DS: Impossibile connettere il file" + args[i] + " alla porta " + port + ": errore di porta");
            } catch (FileNotFoundException fE) {
                System.out.println(
                        "DS: Impossibile connettere il file" + args[i] + " alla porta " + port + ": errore di file");
            }
            i += 2;
        }

        // Routine di lavoro
        while (true) {
            try {
                // Attesa di richiesta del Client
                DatagramSocket clientSocket = new DatagramSocket(dsPort);
                System.out.println("DS: creato nuovo socket sulla porta " + dsPort);
                byte[] clientComunicationBuffer = new byte[256];
                DatagramPacket clientPacket = new DatagramPacket(clientComunicationBuffer,
                        clientComunicationBuffer.length);

                clientPacket.setData(clientComunicationBuffer);
                clientSocket.receive(clientPacket);

                ByteArrayInputStream biStream = new ByteArrayInputStream(clientPacket.getData(), 0,
                        clientPacket.getLength());
                DataInputStream diStream = new DataInputStream(biStream);
                String nomefile = diStream.readUTF();
                System.out.println("DS: " + clientPacket.getSocketAddress() + " asked portnumber of " + nomefile);

                i = 1;
                port = -1;
                while (i < args.length) {
                    if (args[i].equals(nomefile) && socketIsValid[(i - 1) / 2])
                        port = Integer.parseInt(args[i + 1]);
                    i += 2;
                }

                // Invio porta al Client
                ByteArrayOutputStream boStream = new ByteArrayOutputStream();
                DataOutputStream doStream = new DataOutputStream(boStream);

                doStream.writeUTF(Integer.toString(port));
                clientComunicationBuffer = boStream.toByteArray();

                clientPacket.setData(clientComunicationBuffer);
                clientSocket.send(clientPacket);
                System.out.println("DS: Comunicato a " + clientPacket.getAddress() + " il numero " + port);

                clientSocket.disconnect();
                clientSocket.close();

            } catch (IOException e) {
                System.out.println("DS: Error on client communication");
                System.exit(COMMUNICATION_ERROR);
            }
        }
    }

}
