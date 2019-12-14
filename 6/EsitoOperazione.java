import java.io.Serializable;

public class EsitoOperazione implements Serializable {
	static final long serialVersionUID = 69421;
	
	private String fileName;
	private int nLine;

	public String getFileName() {
		return fileName;
	}
	public int getnLine() {
		return nLine;
	}
	public EsitoOperazione(String fileName, int nLine) {
		super();
		this.fileName = fileName;
		this.nLine = nLine;
	}
}
