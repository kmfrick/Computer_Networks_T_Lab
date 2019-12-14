import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class FatherServer {
	public static void main(String[] args) {
		int port = -1;
		// Controllo argomenti
		try {
			if (args.length != 1) {
				System.out.println("Usage: java FatherServer [Port]");
				System.exit(1);
			}
			port = Integer.parseInt(args[0]);
			if (port < 1024 && port > 65536) {
				System.err.println("FS: Porta non valida.");
				System.exit(2);
			}
		} catch (NumberFormatException e) {
			System.err.println("FS: Numero mal formattato");
			System.exit(2);
		}

		try (ServerSocket serverSocket = new ServerSocket(port)) {
			serverSocket.setReuseAddress(true);
			// ciclo principale
			while (true) {
				try {
					Socket clientSocket = serverSocket.accept();
					clientSocket.setSoTimeout(30000);
					ChildServer cs = new ChildServer(clientSocket);
					cs.start();
				} catch (IOException e) {
					System.err.println("FS: Errore I/O.");
					continue;
				}
			}
		} catch (IOException e) {
			System.err.println("FS: Errore apertura server socket");
			System.exit(3);
		}
	}
}
