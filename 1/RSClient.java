import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.StringTokenizer;

public class RSClient {

	public static void main(String[] args) {
		InetAddress addr = null;
		int portDS = 0;
		String filename = null;
		DatagramSocket socket=null;
		DatagramPacket packet=null;
		ByteArrayOutputStream boStream;
		DataOutputStream doStream;
		ByteArrayInputStream biStream;
		DataInputStream diStream;
		byte[] data;
		int portRS=-1;
		try {
			// Controllo argomenti
			if (args.length == 3) {
				addr = InetAddress.getByName(args[0]);
				portDS = Integer.parseInt(args[1]);
				filename = args[2];
			} else {
				System.out.println("Usage: java RSClient IPDS portDS fileName");
				System.exit(1);
			}
			}catch(NumberFormatException e) {
				System.err.println("Numero mal formattato");
				System.exit(8);
			} catch (UnknownHostException e) {
				System.err.println("Errore indirizzo");
				System.exit(9);
			}

			try {
			// Crea la socket e si prepara a comunicare
			socket = new DatagramSocket();
			byte[] buf = new byte[256];
			packet = new DatagramPacket(buf, buf.length, addr, portDS);
			boStream = new ByteArrayOutputStream();
			doStream = new DataOutputStream(boStream);
			doStream.writeUTF(filename);
			data = boStream.toByteArray();
			// Chiede la porta a DS
			packet.setData(data);
			//System.out.println("RSC: invio " + filename);
			socket.send(packet);
			//System.out.println("RSC: inviato " + filename);
			packet.setData(buf);
			socket.receive(packet);
			
			biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
			diStream = new DataInputStream(biStream);
			String readUTF = diStream.readUTF();
			System.out.println("RSC: ricevuto " + readUTF);
			portRS = Integer.parseInt(readUTF);
			if (portRS < 0) { // Il discovery server lancia un valore negativo se non trova il file
				System.err.println("File non trovato.");
				System.exit(3);
			}
			}catch(NumberFormatException e) {
				System.err.println("Numero mal formattato da DS");
				System.exit(10);
			}	 catch (IOException e) {
				System.err.println("Errore io");
				System.exit(11);
				
			}
			packet.setPort(portRS);// l'indirizzo in questo caso non cambia
			System.out.println("RSC: invio su " + portRS);
			String inputl;
			
			BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
			System.out.println("RSC: Inserisci r1 r2 (EOF per uscire)");
			try {
			while((inputl= in.readLine())!=null) { // manda richieste a RS
				int r1, r2;	
				StringTokenizer tokenizer = new StringTokenizer(inputl); // Formato: "r1 r2"
				r1 = Integer.parseInt(tokenizer.nextToken());
				r2 = Integer.parseInt(tokenizer.nextToken());
				if(r1<=0||r2<=0) {
					System.out.println("RSC: Righe sbagliate");
				}else {
					System.out.println("RSC: Letto " + r1 + " " + r2);
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					String sentData = Integer.toString(r1) + " " + Integer.toString(r2);
					doStream.writeUTF(sentData);
					data = boStream.toByteArray();
					// Richiede lo scambio di due righe a RS server
					packet.setData(data);
					//System.out.println("RSC: invio " + sentData);
					socket.send(packet);
					//System.out.println("RSC: inviato " + sentData);
					byte[] buf1 = new byte[256];
					packet.setData(buf1);
					socket.receive(packet);
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);
					String readUTF = diStream.readUTF();
					//System.out.println("RSC: Ricevuto " + readUTF);
					int esito = Integer.parseInt(readUTF);
					//System.out.println("RSC: Esito = " + esito);
					if (esito == 0) { // Viene restituito 0 se l'operazione fallisce
						System.out.println("RSC: Operazione non riuscita.");
					} else {
						System.out.println("RSC: Operazione riuscita.");
					}
					System.out.println("RSC: Inserisci r1 r2 (EOF per uscire)");
				}
			}
			System.err.println("RSC: Stream in input chiuso.");
			in.close();
			socket.close();
		} catch (IOException e) {
			System.err.println("RSC: Errore io");
			System.exit(2);
			
		} catch (NumberFormatException e) {
			System.err.println("RSC: Numero mal formattato");
			System.exit(4);
		}
	}
}