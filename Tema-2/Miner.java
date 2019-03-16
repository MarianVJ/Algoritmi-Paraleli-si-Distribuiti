import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Set;

/**
 * Class for a miner.
 */
public class Miner extends Thread {

	Integer hashCount;
	Set<Integer> solved;
	CommunicationChannel channel;

	// Am copiat aceasta functie din fisiserul Main (folderul Solver) si
	// realizeaza aceasta operatie de hashCount-ori
	private String encryptThisString(String input) {
		for (int j = 0; j < hashCount; j++) {
			try {
				MessageDigest md = MessageDigest.getInstance("SHA-256");
				byte[] messageDigest = md.digest(input.getBytes(StandardCharsets.UTF_8));

				// convert to string
				StringBuffer hexString = new StringBuffer();
				for (int i = 0; i < messageDigest.length; i++) {
					String hex = Integer.toHexString(0xff & messageDigest[i]);
					if (hex.length() == 1)
						hexString.append('0');
					hexString.append(hex);
				}
				input = hexString.toString();

			} catch (NoSuchAlgorithmException e) {
				throw new RuntimeException(e);
			}
		}
		return input;
	}

	/**
	 * Creates a {@code Miner} object.
	 * 
	 * @param hashCount
	 *            number of times that a miner repeats the hash operation when
	 *            solving a puzzle.
	 * @param solved
	 *            set containing the IDs of the solved rooms
	 * @param channel
	 *            communication channel between the miners and the wizards
	 */
	public Miner(Integer hashCount, Set<Integer> solved, CommunicationChannel channel) {
		this.hashCount = hashCount;
		this.solved = solved;
		this.channel = channel;
	}

	@Override
	public void run() {

		// Mesajul in care se afla numarul tatalui
		Message message = null;
		// Mesajul in care se afla numarul fiului
		Message message2 = null;

		while (true) {

			synchronized (channel.wizard) {

				message = channel.getMessageWizardChannel();
				// Conditia de oprire
				if (!message.getData().equals("EXIT"))
					message2 = channel.getMessageWizardChannel();
				else {
					break;
				}
				
			}
			
			//Pentru a nu trimite de 2 ori spre rezolvare (descoperire a vecinilor)
			//aceeasi incapere 
			//Verificam daca ea a fost trimisa inainte ( iar daca a fost calculata
			//a fost si trimisa inapoi la wizard pt a debloca noi incaperi)
			if (!this.solved.contains(message2.getCurrentRoom())) {
				solved.add(message2.getCurrentRoom());
				channel.putMessageMinerChannel(new Message(message.getCurrentRoom(), message2.getCurrentRoom(),
						this.encryptThisString(message2.getData())));
			}
		}
	}

}
