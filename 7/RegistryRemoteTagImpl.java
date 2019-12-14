
/**
 * 	Implementazione del Registry Remote.
 *	Metodi descritti nelle interfacce.  
 */

import java.rmi.Naming;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.RMISecurityManager;
//@SuppressWarnings("deprecation")
public class RegistryRemoteTagImpl extends UnicastRemoteObject implements RegistryRemoteTagServer {

	/**
	 * 
	 */
	private static final long serialVersionUID = 2L;
	// num. entry [nomelogico][ref]
	final int tableSize = 100;
	final int nTags = 10;
	// Tabella: la prima colonna contiene i nomi, la seconda i riferimenti remoti
	Object[][] table = new Object[tableSize][2];
	String[][] tagAss = new String[tableSize][2];
	String[] tags = new String[nTags];

	public RegistryRemoteTagImpl() throws RemoteException {
		super();
		for (int i = 0; i < tableSize; i++) {
			table[i][0] = null;
			table[i][1] = null;
			tagAss[i][0] = null;
			tagAss[i][1] = null;
		}
		for (int i = 0; i < nTags; i++)
			tags[i] = i + "";
	}

	/** Aggiunge la coppia nella prima posizione disponibile */
	public synchronized boolean aggiungi(String nomeLogico, Remote riferimento) throws RemoteException {
		// Cerco la prima posizione libera e la riempio
		boolean result = false;
		if ((nomeLogico == null) || (riferimento == null))
			return result;
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] == null) {
				table[i][0] = nomeLogico;
				table[i][1] = riferimento;
				result = true;
				break;
			}
		return result;
	}

	public synchronized boolean associaTag(String nomeLogico, String tag) throws RemoteException {
		if (nomeLogico == null)
			return false;
		int i;
		for(i=0; i<nTags&& !tags[i].equals(tag); i++);
		if(i==nTags)
			throw new RemoteException("Tag sbagliato");
		for (i=0; i<tableSize && !table[i][0].equals(nomeLogico); i++);	
		if (i == tableSize) {
			throw new RemoteException("Nome logico sbagliato!");
		}
		for (i = 0; i < tableSize; i++) {
				if(tagAss[i][0]==null) {
					tagAss[i][0]=nomeLogico;
					tagAss[i][1]=tag;
					return  true;
				}
		}
		return false;
	}

	/** Restituisce il riferimento remoto cercato, oppure null */
	public synchronized Remote cerca(String nomeLogico) throws RemoteException {
		Remote result = null;
		if (nomeLogico == null)
			return null;
		for (int i = 0; i < tableSize; i++)
			if (nomeLogico.equals((String) table[i][0])) {
				result = (Remote) table[i][1];
				break;
			}
		return result;
	}

	/** Restituisce tutti i riferimenti corrispondenti ad un nome logico */
	public synchronized Remote[] cercaTutti(String nomeLogico) throws RemoteException {
		int cont = 0;
		if (nomeLogico == null)
			return new Remote[0];
		for (int i = 0; i < tableSize; i++)
			if (nomeLogico.equals((String) table[i][0]))
				cont++;
		Remote[] result = new Remote[cont];
		// Ora lo uso come indice per il riempimento
		cont = 0;
		for (int i = 0; i < tableSize; i++)
			if (nomeLogico.equals((String) table[i][0]))
				result[cont++] = (Remote) table[i][1];
		return result;
	}

	/** Restituisce tutti i riferimenti corrispondenti ad un tag */
	public synchronized String[] cercaTag(String tag) throws RemoteException {
		int cont = 0;
		if (tag == null)
			return new String[0];
		int i;
		for (i = 0; i < nTags && !tags[i].equals(tag); i++)
			;
		if (i == nTags)
			throw new RemoteException("Tag sbagliato");
		for (i = 0; i < tableSize; i++)
			if (tag.equals((String) tagAss[i][1]))
				cont++;
		String[] result = new String[cont];
		// Ora lo uso come indice per il riempimento
		cont = 0;
		for (i = 0; i < tableSize; i++)
			if (tag.equals((String) tagAss[i][1]))
				result[cont++] = (String) tagAss[i][0];
		return result;
	}

	/** Restituisce tutti i riferimenti corrispondenti ad un nome logico */
	public synchronized Object[][] restituisciTutti() throws RemoteException {
		int cont = 0;
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] != null)
				cont++;
		Object[][] result = new Object[cont][2];
		// Ora lo uso come indice per il riempimento
		cont = 0;
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] != null) {
				result[cont][0] = table[i][0];
				result[cont][1] = table[i][1];
				result[cont][0] = table[i][2];
			}
		return result;
	}

	/** Elimina la prima entry corrispondente al nome logico indicato */
	public synchronized boolean eliminaPrimo(String nomeLogico) throws RemoteException {
		boolean result = false;
		if (nomeLogico == null)
			return result;
		for (int i = 0; i < tableSize; i++)
			if (nomeLogico.equals((String) table[i][0])) {
				table[i][0] = null;
				table[i][1] = null;
				table[i][2] = null;
				result = true;
				break;
			}
		return result;
	}

	public synchronized boolean eliminaTutti(String nomeLogico) throws RemoteException {
		boolean result = false;
		if (nomeLogico == null)
			return result;
		for (int i = 0; i < tableSize; i++)
			if (nomeLogico.equals((String) table[i][0])) {
				if (result == false)
					result = true;
				table[i][0] = null;
				table[i][1] = null;
				table[i][2] = null;
			}
		return result;
	}

	// Avvio del Server RMI
	
	public static void main(String[] args) {
		System.setProperty("java.security.policy","file:./rmi.policy");
		int registryRemotePort = 1099;
		String registryRemoteHost = "localhost";
		String registryRemoteName = "RegistryRemote";

		// Controllo dei parametri della riga di comando
		if (args.length != 0 && args.length != 1) {
			System.out.println("Sintassi: ServerImpl [registryPort]");
			System.exit(1);
		}
		if (args.length == 1) {
			try {
				registryRemotePort = Integer.parseInt(args[0]);
			} catch (Exception e) {
				System.out.println("Sintassi: ServerImpl [registryPort], registryPort intero");
				System.exit(2);
			}
		}

		// Impostazione del SecurityManager
		if (System.getSecurityManager() == null)
			System.setSecurityManager(new RMISecurityManager());

		// Registrazione del servizio RMI
		String completeName = "//" + registryRemoteHost + ":" + registryRemotePort + "/" + registryRemoteName;
		try {
			RegistryRemoteTagImpl serverRMI = new RegistryRemoteTagImpl();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + registryRemoteName + "\" registrato");
		} catch (Exception e) {
			System.err.println("Server RMI \"" + registryRemoteName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}