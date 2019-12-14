import java.rmi.RemoteException;

public interface RegistryRemoteTagClient extends RegistryRemoteClient {
	public String[] cercaTag(String tag) throws RemoteException;
}
