import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;

public class Produttore {

	public static void main(String[] args) {
		// Controlo argomenti
		if (args.length != 1) {
			System.out.println("Utilizzo: produttore <inputFilename>");
			System.exit(0);
		}
		// Lettura da stdin
		try (BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
				// Scrittura su file 
				FileWriter fout = new FileWriter(args[0])) { // Nome file come argomento 0

			String inputl;
			while ((inputl = in.readLine()) != null) {  // Lettura riga per riga
				inputl += "\n";
				fout.write(inputl, 0, inputl.length()); // Scrive riga su file
			}
		} catch (IOException e) { // Gestione eccezioni
			e.printStackTrace();
			System.exit(2);
		}
	}
}
