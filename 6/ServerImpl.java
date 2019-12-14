import java.io.*;
import java.rmi.*;
import java.rmi.server.*;
import java.util.*;

public class ServerImpl extends UnicastRemoteObject implements RemOp {
    static final long serialVersionUID = 69420;

    public ServerImpl() throws RemoteException {
        super();
    }

    public static void main(String[] args) {
        int registryPort = 1099;
        String registryHost = "localhost";
        String serviceName = "ServerRMI";

        // Controllo dei parametri della riga di comando
        if (args.length != 0 && args.length != 1) {
            System.out.println("Usage: " + args[0] + " [REGISTRYPORT]");
            System.exit(1);
        }
        if (args.length == 1) {
            try {
                registryPort = Integer.parseInt(args[0]);
            } catch (Exception e) {
                System.out.println("Usage: " + args[0] + " [REGISTRYPORT]");
                System.err.println("REGISTRYPORT deve essere un intero!");
                System.exit(2);
            }
        }

        // Registrazione del servizio
        String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;

        try {
            ServerImpl serverRMI = new ServerImpl();
            Naming.rebind(completeName, serverRMI);
            System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");
        } catch (Exception e) {
            System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }

    public int conta_righe(String fileName, int thresh) throws RemoteException {
        int result = 0;
        try (BufferedReader r = new BufferedReader(new FileReader(fileName))) {
            String line;
            while ((line = r.readLine()) != null) {
                StringTokenizer t = new StringTokenizer(line);
                if (t.countTokens() > thresh)
                    result++;
            }
        } catch (IOException e) {
            throw new RemoteException("Errore di I/O");
        }
        return result;
    }

    public synchronized EsitoOperazione elimina_riga(String fileName, int nElim) throws RemoteException {
        int i = 1;
        String newFName = fileName + ".1";
        try (BufferedReader r = new BufferedReader(new FileReader(fileName));
                FileWriter fw = new FileWriter(newFName)) {
            String line;
            while ((line = r.readLine()) != null) {
                if (i != nElim) {
                    fw.write(line);
                    fw.write('\n');
                }
                i++;
            }
            if (i <= nElim) {
                throw new RemoteException("Riga non presente nel file");
            }

        } catch (IOException e) {
            throw new RemoteException("Errore di I/O");
        }
        return new EsitoOperazione(newFName, i - 2);
    }

}
