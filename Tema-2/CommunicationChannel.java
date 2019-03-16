import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Class that implements the channel used by wizards and miners to communicate.
 */
public class CommunicationChannel {

	ArrayBlockingQueue<Message> miner = new ArrayBlockingQueue<Message>(1002);
	ArrayBlockingQueue<Message> wizard = new ArrayBlockingQueue<Message>(1002);
	
	//Presupun ca nu vor fi mai mult de 100 de threaduri in total 
	Message[] m = new Message[100];

	/**
	 * Creates a {@code CommunicationChannel} object.
	 */
	public CommunicationChannel() {
	}

	/**
	 * Puts a message on the miner channel (i.e., where miners write to and wizards
	 * read from).
	 * 
	 * @param message
	 *            message to be put on the channel
	 */
	public void putMessageMinerChannel(Message message) {
		try {
			miner.put(message);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

	}

	/**
	 * Gets a message from the miner channel (i.e., where miners write to and
	 * wizards read from).
	 * 
	 * @return message from the miner channel
	 */
	public Message getMessageMinerChannel() {

		Message message = null;
		try {
			message = miner.take();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return message;
	}

	/**
	 * Puts a message on the wizard channel (i.e., where wizards write to and miners
	 * read from).
	 * 
	 * @param message
	 *            message to be put on the channel
	 */
	
	public void putMessageWizardChannel(Message message) {

		int id = (int) Thread.currentThread().getId();
		//Deoarece mesajele de "END" nu sunt folosite in niciun fel in Mineri 
		//Nu mai sunt trimise la acesta 
		if (message.getData().equals("END") || message.getData().equals("EXIT")) {
				if (message.getData().equals("EXIT"))
					try {
						wizard.put(message);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					
		} else {
			if (m[id] == null) {
				m[id] = message;
			} else {
				try {
					//Preiau controlul o singura data pentru fiecare 
					//pereche de mesaje ce contin numarului tatalui si al descnedentului
					//si le pun pe canalul de comunicatie 
					synchronized (this) {
						wizard.put(m[id]);
						wizard.put(message);
					}
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				m[id] = null;
			}
		}
	}

	/**
	 * Gets a message from the wizard channel (i.e., where wizards write to and
	 * miners read from).
	 * 
	 * @return message from the miner channel
	 */
	public Message getMessageWizardChannel() {
		Message message = null;
		try {
			message = wizard.take();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		return message;
	}
}
