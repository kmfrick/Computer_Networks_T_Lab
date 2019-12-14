import java.rmi.RemoteException;
public interface RegistryRemoteTagServer extends RegistryRemoteServer, RegistryRemoteTagClient {
    public boolean associaTag(String nome_logico_server, String tag) throws RemoteException;
}