
import java.rmi.Remote;
import java.rmi.RemoteException;


public interface RemOp extends Remote {
	public synchronized EsitoOperazione elimina_riga(String fileName, int nElim) throws RemoteException;
	public int conta_righe(String fileName, int thresh) throws RemoteException;	
}
