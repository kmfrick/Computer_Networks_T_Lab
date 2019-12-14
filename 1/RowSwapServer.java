import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.Random;
import java.util.StringTokenizer;

public class RowSwapServer extends Thread {
    private static final int SIZE = 256;
    private byte[] buf = new byte[SIZE];
    private String filename, outfile;
    private String firstLine, secondLine;
    private int a, b;
    private int port;	// Ogni RSS ha la propria porta.
						// Il caso in cui due RSS vengono creati
					    // con la stessa porta è gestito dal DS, non qui.
    private int success;
    private DatagramSocket socket;
    private DatagramPacket packet; // Avere il pacchetto come membro evita di doverlo
                                   // creare ogni volta: risparmio di memoria.
    private FileInputStream fiStream;
    private BufferedReader reader;
    private BufferedWriter writer;

    @Override
    public void run() {

        int i = 0;
        String line;
        String request;
        while (true) {
            this.success = 1;
            try {
                this.fiStream = new FileInputStream(filename);
                this.fiStream.getChannel().position(0);
            } catch (FileNotFoundException e) {
                System.err.println("RSS: " + filename + ": file non trovato.");
                this.success = 0;
                this.sendReturnPacket();
                
            } catch (IOException e) {
                System.err.println("RSS: Errore nella reimpostazione dello stream.");
                this.success = 0;
                this.sendReturnPacket();
            }

            // Ricevi pacchetto
            if (this.success > 0) {
                try {
                    
                    this.socket.receive(this.packet);
                } catch (IOException e) {
                    System.err.println("RSS: Errore durante la ricezione del pacchetto.");
                    this.success = 0;
                    this.sendReturnPacket();
                }
            }

            // Elabora contenuto pacchetto
            if (this.success > 0) {
                ByteArrayInputStream biStream = new ByteArrayInputStream(this.packet.getData(), 0,
                        this.packet.getLength());
                DataInputStream diStream = new DataInputStream(biStream);
                try {
                    request = diStream.readUTF();
                    System.out.println("RSS: Ricevute righe " + request);
                    StringTokenizer tokenizer = new StringTokenizer(request); // Formato: "a b"
                    this.a = Integer.parseInt(tokenizer.nextToken());
                    this.b = Integer.parseInt(tokenizer.nextToken());

                    // Forzare a < b rende più semplice la gestione
                    if (this.a > this.b) {
                        int t = this.a;
                        this.a = this.b;
                        this.b = t;
                    }
                    this.a--;
                    this.b--;

                } catch (IOException e) {
                    System.err.println("RSS: Errore durante il parsing dei dati del pacchetto.");
                    this.success = 0;
                    this.sendReturnPacket();
                }
            }

            if (this.success > 0) {
                this.reader = new BufferedReader(new InputStreamReader(this.fiStream));

                // Trova le righe da scambiare
                i = 0;
                while (i < this.b) {
                    try {
                        line = this.reader.readLine();
                        if (line == null)
                            throw new IOException();
                        else if (i == this.a) {
                            this.firstLine = line;
                        }
                    } catch (IOException e) {
                        System.err.println("RSS: Incontrata fine file prematura.");
                        this.success = 0;
                        i = b + 1;
                        this.sendReturnPacket();
                    }
                    i++;
                }
            }
            if (this.success > 0) {
                try {
                    if (a != b)
                        secondLine = reader.readLine();
                    else
                        secondLine = firstLine;
                    if (secondLine == null)
                        throw new IOException();
                } catch (IOException e) {
                    System.err.println("RSS: Incontrata fine file prematura nella lettura della seconda riga.");
                    this.success = 0;
                    this.sendReturnPacket();
                }
            }
            // Leggi il file e stampa le righe
            // Intercettando le righe in esame e scambiandole
            if (this.success > 0) {
                //System.out.println("firstLine: " + firstLine);
                //System.out.println("secondLine: " + secondLine);
                i = 0;
                try {
                    this.fiStream.getChannel().position(0);
                    reader = new BufferedReader(new InputStreamReader(this.fiStream));
                    writer = new BufferedWriter(new FileWriter(this.outfile));

                    while ((line = this.reader.readLine()) != null) {
                        if (i == this.a) {
                            System.out.println(this.secondLine);
                            this.writer.write(this.secondLine);
                        } else if (i == this.b) {
                            System.out.println(this.firstLine);
                            this.writer.write(this.firstLine);
                        } else {
                            System.out.println(line);
                            this.writer.write(line);
                        }
                        this.writer.newLine(); // Portabilità, invece di \n
                        i++;
                    }
                    this.fiStream.getChannel().position(0);
                } catch (IOException e) {
                    System.err.println("RSS: Errore in scrittura.");
                    e.printStackTrace();
                    this.success = 0;
                    this.sendReturnPacket();
                }
            }
            // Sostituisci il file originale con quello con le righe scambiate
            if (this.success > 0) {
                try {
                    File curFile = new File(filename);
                    if (curFile.delete()) {
                        File newFile = new File(outfile);
                        if (!newFile.renameTo(curFile))
                            throw new IOException();
                    } else
                        throw new IOException();
                } catch (IOException e) {
                    System.err.println("RSS: Errore nella rinominazione del file.");
                    this.success = 0;
                    this.sendReturnPacket();
                }
            }
            if (this.success > 0)
                this.sendReturnPacket();
        }
    }

    private void sendReturnPacket() {
        try {
            this.reader.close();
            if (writer != null)
                this.writer.close();
            ByteArrayOutputStream boStream = new ByteArrayOutputStream();
            DataOutputStream doStream = new DataOutputStream(boStream);
            try {
                doStream.writeUTF(Integer.toString(this.success));
                this.packet.setData(boStream.toByteArray());
                Thread.sleep(500); // Aspetta che il client sia in attesa
                this.socket.send(this.packet);
                //System.out.println("RSS: Inviato " + Integer.toString(this.success));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            System.err.println("RSS: Operazione di scambio " + (this.success > 0 ? "" : "non") + " riuscita");
            this.packet.setData(buf);
            this.packet.setLength(SIZE);
        } catch (IOException e) {
            System.err.println("RSS: Errore nell'invio del pacchetto di risposta.");
            e.printStackTrace();
        }
    }

    public RowSwapServer(String filename, int port) throws SocketException, FileNotFoundException {
        System.out.println("RSS: Ricevuto " + filename + " " + port);
        File file = new File(filename);
        if (!file.exists() || file.length() == 0) {
            throw new FileNotFoundException(); }
        this.socket = new DatagramSocket(port);
        this.packet = new DatagramPacket(buf, SIZE);
        this.filename = filename;
        this.port = port;
        Random r = new Random(System.currentTimeMillis());
        this.outfile = filename + ".out" + r.nextInt(256);
        this.success = 0;
    }

    public int getPort() {
        return this.port;
    }

    public int getSuccess() {
        return success; // run() non può restituire valori
    }

}
