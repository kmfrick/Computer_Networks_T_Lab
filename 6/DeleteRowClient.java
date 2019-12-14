import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

public class DeleteRowClient {

    public static void main(String[] args) {
        int registryPort = 1099;
        String registryHost = null; // host remoto con registry
        String serviceName = "ServerRMI"; // lookup name...Hardcoded
        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

        // Controllo dei parametri della riga di comando
        if (args.length != 2) {
            System.out.println("Sintassi: Client RegistryHost [registryPort]");
            System.exit(1);
        }

        registryHost = args[0];

        try {
            registryPort = Integer.parseInt(args[1]);
        } catch (Exception e) {
            System.out.println("Sintassi: Client RegistryHost [registryPort]");
            System.err.println("registryPort deve essere un intero!");
            System.exit(2);
        }

        System.out.println(
                "Invio richieste a " + registryHost + ":" + registryPort + " per il servizio di nome " + serviceName);

        // Connessione al servizio RMI remoto
        try {
            String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;
            RemOp serverRMI = (RemOp) Naming.lookup(completeName);
            System.out.println("ClientRMI: Servizio \"" + serviceName + "\" connesso");
            String nomeFile;
            System.out.println("Inserisci il nome del file: ");
            while ((nomeFile = stdIn.readLine()) != null) {
                int numParole;
                try {
                    System.out.println("Inserisci il numero di riga da eliminare: ");
                    numParole = Integer.parseInt(stdIn.readLine());
                    if (numParole < 1) {
                        throw new NumberFormatException();
                    }
                } catch (NumberFormatException e) {
                    System.out.println("Deve essere un intero maggiore di 0!");
                    System.out.println("Inserisci il nome del file: ");
                    continue;
                }
                EsitoOperazione result = serverRMI.elimina_riga(nomeFile, numParole);
                System.out.println("É stato creato un nuovo file di " + result.getnLine() + " righe di nome " + result.getFileName());
                System.out.println("Inserisci il nome del file: ");
            }
        } catch (NotBoundException nbe) {
            System.err.println("ClientRMI: il nome fornito non risulta registrato; " + nbe.getMessage());
            // nbe.printStackTrace();
            System.exit(1);
        } catch (RemoteException e) {
            System.err.println("ClientRMI: " + e.getMessage());
            System.exit(69);
        } catch (Exception e) {
            System.err.println("ClientRMI: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }
}