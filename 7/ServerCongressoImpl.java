/**
 * ServerCongressoImpl.java
 * 		Implementazione del server
 * */

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.RMISecurityManager;
//@SuppressWarnings("deprecation")
public class ServerCongressoImpl extends UnicastRemoteObject implements
    ServerCongresso {

	private static final long serialVersionUID = 1L;
static Programma prog[];

  // Costruttore
  public ServerCongressoImpl() throws RemoteException {
    super();
  }

  // Richiede una prenotazione
  public int registrazione(int giorno, String sessione, String speaker)
      throws RemoteException {
    int numSess = -1;
    System.out.println("Server RMI: richiesta registrazione con parametri");
    System.out.println("giorno   = " + giorno);
    System.out.println("sessione = " + sessione);
    System.out.println("speaker  = " + speaker);

    if (sessione.startsWith("S")) {
      try {
        numSess = Integer.parseInt(sessione.substring(1)) - 1;
      } catch (NumberFormatException e) {
      }
    }

    // Se i dati sono sbagliati significa che sono stati trasmessi male e quindi
    // solleva una eccezione
    if (numSess == -1)
      throw new RemoteException();
    if (giorno < 1 || giorno > 3)
      throw new RemoteException();

    return prog[giorno - 1].registra(numSess, speaker);
  }

  // Ritorno il campo
  public Programma programma(int giorno) throws RemoteException {
    System.out.println("Server RMI: richiesto programma del giorno " + giorno);
    return prog[giorno - 1];
  }

  // Avvio del Server RMI

public static void main(String[] args) {
  System.setProperty("java.security.policy","file:./rmi.policy");
    // creazione programma
    prog = new Programma[3];
    for (int i = 0; i < 3; i++)
      prog[i] = new Programma();
    int registryRemotePort = 1099;
    String registryRemoteName = "RegistryRemote";
    String serviceName = "ServerCongresso";
    String tag = "1";

    // Controllo dei parametri della riga di comando
    if (args.length != 1 && args.length != 2) {
      System.out
          .println("Sintassi: ServerCongressoImpl NomeHostRegistryRemote [registryPort], registryPort intero");
      System.exit(1);
    }
    String registryRemoteHost = args[0];
    if (args.length == 2) {
      try {
        registryRemotePort = Integer.parseInt(args[1]);
      } catch (Exception e) {
        System.out
            .println("Sintassi: ServerCongressoImpl NomeHostRegistryRemote [registryPort], registryPort intero");
        System.exit(2);
      }
    }

    // Impostazione del SecurityManager
    if (System.getSecurityManager() == null) {
      System.setSecurityManager(new RMISecurityManager());
    }

    // Registrazione del servizio RMI
    String completeRemoteRegistryName = "//" + registryRemoteHost + ":"
        + registryRemotePort + "/" + registryRemoteName;

    try {
      RegistryRemoteTagServer registryRemote = (RegistryRemoteTagServer) Naming
          .lookup(completeRemoteRegistryName);
      ServerCongressoImpl serverRMI = new ServerCongressoImpl();
      registryRemote.aggiungi(serviceName, serverRMI);
      System.out.println("Server RMI: Servizio \"" + serviceName
          + "\" registrato");
      
      registryRemote.associaTag(serviceName, tag);
      
      System.out.println("Tag associato.");
      
    } catch (Exception e) {
      System.err.println("Server RMI \"" + serviceName + "\": "
          + e.getMessage());
      e.printStackTrace();
      System.exit(1);
    }
  }
}