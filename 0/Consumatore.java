import java.io.*;

public class Consumatore {

    public static void main(String[] args) {
        // Controllo argomenti
        if (args.length != 1 && args.length != 2) {
            System.out.println("Utilizzo: produttore filter [<inputFilename>]");
            System.exit(0);
        }

        Reader reader = null;
        String filter = args[0];
        // Legge da stdin se non viene dato il nome file
        if (args.length == 1) {
            reader = new InputStreamReader(System.in);
        } else { // Altrimenti legge da file
            try {
                reader = new FileReader(args[1]);
            } catch (FileNotFoundException e) {
                System.out.println("File non trovato");
                System.exit(1);
            }
        }

        try (BufferedReader r = new BufferedReader(reader)) {
            int x; // Rappresentazione numerica carattere letto
                   // Per poter restituire -1 in caso di EOF
            char ch;
            while ((x = r.read()) >= 0) {
                ch = (char) x;

                if (!filter.contains(ch + "")) // Stampa solo se il carattere non è contenuto nella stringa filtro
                    System.out.print(ch);
            }
            reader.close();
            // Gestione eccezioni
        } catch (IOException ex) {
            System.out.println("Errore di input");
            System.exit(2);
        }
    }
}
