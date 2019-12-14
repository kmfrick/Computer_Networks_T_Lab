import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {
	private static final String FILE_VALID = "attiva";
	private static final String FILE_INVALID = "salta file";
	private static File directory;
	private static Long minSize;
	private static Integer port;
	private static InetAddress addr;
	private static Socket socket;

	public static void main(String[] args) {

		// Controllo argomenti
		if (args.length != 2) {
			System.err.println("Usage: Client [Addr] [Port]");
			System.exit(1);
		}

		try {
			addr = InetAddress.getByName(args[0]);
			port = Integer.parseInt(args[1]);
			if (port < 1024 && port > 65536) {
				System.err.println("C: Porta non valida.");
				System.exit(2);
			}
		} catch (UnknownHostException e1) {
			System.err.println("C: Indirizzo non valido.");
		}

		try (BufferedReader in = new BufferedReader(new InputStreamReader(System.in))) {
			while (true) {
				readInput(in);
				try {
					socket = new Socket(addr, port);
					DataInputStream sockIn = new DataInputStream(socket.getInputStream());
					DataOutputStream sockOut = new DataOutputStream(socket.getOutputStream());

					for (File file : directory.listFiles()) {
						moveFile(sockIn, sockOut, file);
					}
				} catch (IOException e) {
					System.exit(5);
				}

				try {
					socket.shutdownInput();
					socket.shutdownOutput();
					socket.close();
				} catch (IOException e) {
					System.err.println("C: Errore nella chiusura della socket.");
					System.exit(10);
				}
			}
		} catch (EOFException e) {
			System.out.println("EOF");
		} catch (IOException e) {
			System.err.println("C: Errore nella lettura dei dati in input.");
			System.exit(3);
		}
	}

	private static void readInput(BufferedReader in) throws IOException {

		System.out.println("C: Inserire il direttorio da trasferire: ");
		directory = new File(in.readLine());
		if (!directory.isDirectory()) {
			System.err.println("C: " + directory + " non è un direttorio.");
			return;
		}
		System.out.print("C: Inserire dimensione minima dei file: ");
		minSize = Long.parseLong(in.readLine());
		if (minSize < 0) {
			System.err.println("C: Dimensione minima non valida.");
			System.exit(4);
		}
	}

	private static void moveFile(DataInputStream sockIn, DataOutputStream sockOut, File file) throws IOException {
		if (file.isDirectory() || file.length() < minSize) {
			return;
		}

		System.out.println("C: Invio nome file.");
		sockOut.writeUTF(file.getName());

		System.out.println("C: Chiedo risposta dal server.");
		String risposta = sockIn.readUTF();
		System.out.println("C: Risposta: " + risposta);
		System.out.println("C: Invio dimensione file");
		sockOut.writeLong(file.length());

		if (risposta.equals(FILE_VALID)) {
			try (FileInputStream inFile = new FileInputStream(file)) {
				int byteRead;
				while ((byteRead = inFile.read()) > -1) {
					sockOut.write(byteRead);
				}
				sockOut.flush();
			} catch (IOException e) {
				System.err.println("C: Errore durante il trasferimento del file.");
				System.exit(6);
			}
		} else if (risposta.equals(FILE_INVALID)) {
			System.err.println("C: File già presente sul file system del server.");
		}
		return;
	}

}
