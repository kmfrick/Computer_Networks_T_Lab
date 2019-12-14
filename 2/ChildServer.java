import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.Socket;

public class ChildServer extends Thread {
	private static String FILE_VALID = "attiva";
	private static String FILE_INVALID = "salta file";

	private Socket socket;

	public ChildServer(Socket socket) {
		this.socket = socket;
	}

	@Override
	public void run() {
		int b;
		try {
			DataInputStream sockIn = new DataInputStream(socket.getInputStream());
			DataOutputStream sockOut = new DataOutputStream(socket.getOutputStream());
			String nomeFile;
			while (true) {
				try {
					nomeFile = sockIn.readUTF();
				} catch (EOFException e) {
					System.out.println("CS: Fine direttorio.");
					break;
				}
				// Ricezione nome file
				File f = new File(nomeFile);
				if (f.exists()) {
					sockOut.writeUTF(FILE_INVALID);
					continue;
				} else {
					sockOut.writeUTF(FILE_VALID);
				}

				// Ricezione dimensione file
				long dimFile = sockIn.readLong();
				System.out.println("CS: Dimensione file: " + nomeFile + " = " + dimFile);

				// Creazione file su FS del CS
				try {
					f.createNewFile();
				} catch (FileNotFoundException e) {
					System.out.println("CS: Errore nella creazione del file.");
					break;
				}

				// Ricezione contenuto file
				try (FileOutputStream newFileStream = new FileOutputStream(f)) {
					int i = 0;
					while (i < dimFile) {
						b = sockIn.read();
						newFileStream.write(b);
						i++;
					}
					System.out.println("CS: fine copia file: " + f);
				} catch (IOException e) {
					System.err.println("CS: Errore nella scrittura del file.");
				}
			}

			socket.close();
		} catch (EOFException e) {
			System.err.println("CS: Raggiunto fine stream.");
		} catch (IOException e) {
			System.err.println("CS: Errore di comunicazione.");
			System.exit(10);
		}
	}
}
