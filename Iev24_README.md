**Linux User Namespace Lab Report**

**Part 1: Exploring User Namespaces Basics**
1. current user ID and groups:
![Id](https://github.com/user-attachments/assets/baa1995d-7e17-470c-bde9-db5accc61754)

**2. List of current namespaces:**
![IIDD](https://github.com/user-attachments/assets/cd514a76-4388-44b9-afce-2d18b704ef98)

**3. New user namespace**
![new id](https://github.com/user-attachments/assets/4a5d1e94-a556-4fbf-b293-ac834acd8853)
The UID and GID of 65534 ("nobody") appear due to UID/GID mapping in user namespaces. This allows processes to have root-like privileges within the namespace while being mapped to a non-privileged "nobody" UID/GID outside, ensuring security by isolating the process from the host system.

**4. Namespace identifier to confirm you're in a different user namespace:**
   ![who](https://github.com/user-attachments/assets/fccda07c-dd54-4a24-a6e2-e5d7fa9b5a80)
   When comparing the namespace identifier with `ls -la /proc/self/ns/user`, you’ll see a change in the user namespace, and the UID/GID will show as 65534 (nobody), indicating a different user namespace with isolated privileges.

**5. New namespace's UID mapping:**
   ![catt](https://github.com/user-attachments/assets/d86a94c3-9238-44b3-a2b5-1fb0b556ba0a)
   Files are empty, indicating no mappings are established

**Part 2: Creating User Namespaces with UID/GID Mappings**
![q3](https://github.com/user-attachments/assets/b57112f2-d23d-487a-b5d9-f4b631474aef)

Based on the output of the ./userns_child program, here are the answers:

**1. What is your UID in the new user namespace?**
The child process has a UID of 65534 in the new user namespace. This is a non-privileged user, often mapped to the `nobody` user in the system.

**2. What capabilities do you have in the user namespace?**
The child process has a wide range of capabilities available within the user namespace. These include:
cap_chown, cap_dac_override, cap_fowner, cap_kill, and more.
These capabilities allow the child process to perform administrative tasks within the namespace (e.g., changing ownership, overriding file access controls, managing network settings), but they do not extend to the global system outside the namespace.

**3. How do the UID mappings work?**
UID mappings allow the child process to have a different UID inside the user namespace. Although the process has UID `65534` within the namespace (a non-privileged user), it is mapped to a different UID (often `0` or `root`) when interacting with the global system.
This creates an isolated environment where the child process may have root-like privileges within the namespace but doesn't have global root privileges.


**Part 3: Exploring Isolation with User Namespaces**
![before](https://github.com/user-attachments/assets/cf66de40-a017-4163-ae65-8894da7c790c)

![After](https://github.com/user-attachments/assets/897dcf26-3f50-4d6a-b31c-6ca4aae78b7e)


**Part 4: Practical Applications - Running a Program with "Root" Privileges**
![root](https://github.com/user-attachments/assets/f560656b-325f-4356-a1ec-6caaf92b4b62)

![succ](https://github.com/user-attachments/assets/8aeefecd-b4d0-45b1-b398-fda632159c45)

**Part 5: Exploring the Limitations**
![5a](https://github.com/user-attachments/assets/d8edeb1b-0ec0-4272-ac5c-4d95f2729930)

![5b](https://github.com/user-attachments/assets/6e0a9a11-b53b-4dd5-b099-def123d57622)

**Successfully*

**Deliverables**

1. Linux user namespaces are utility or mechanism that allows procs to have their own namespaces (UIDs and GIDs) within an isolated environment. This means that a process can have root privileges within a specific user namespace, while not affecting the system’s global user permissions. User namespaces provide a method to isolate and control users in a manner that is secure to do one do more tasks, especially in the instances where the full system privileges are not required. This isolation mechanism is paramount to the proper functioning of container technologies by which the processes can have custom UID/GID mappings, while tooling around, access to the host system is deliberately prevented.


**1. How do user namespaces provide the illusion of having root privileges?**
User namespaces provide the illusion of root privileges by allowing processes to have different UID and GID mappings inside the namespace. A process that runs inside a user namespace can appear to have UID 0 (root) within that namespace but is mapped to a non-privileged UID (e.g., UID 65534, or "nobody") outside the namespace. This mapping allows the process to act as root within the namespace without affecting the system outside the namespace.

**2. What is the purpose of UID/GID mapping in user namespaces?**
The purpose of UID/GID mapping is to provide isolated user and group IDs for processes running within a user namespace. This allows processes to have a root-like environment inside the namespace while using non-privileged IDs outside the namespace. This mapping prevents conflicts and unauthorized access between the isolated namespace and the global system.

**3. What limitations did you encounter when working with user namespaces?**
Some limitations when working with user namespaces include:
Certain system capabilities and privileges may not work as expected in the namespace, such as access to devices or kernel-level operations.
There may be restrictions on creating new user namespaces, especially for unprivileged users, unless specific kernel options are enabled or additional configurations are made.
The use of certain namespace features (like file systems or network namespaces) can require elevated permissions or root access.

**4. How might user namespaces be used in container technology?**
In container technology, user namespaces are used to provide each container with its own isolated user environment. This ensures that processes within the container can have root privileges within the container but are mapped to non-privileged users on the host system, preventing security risks. It helps in providing fine-grained access control and resource isolation for containers, making it easier to manage security and permissions across multiple containers.

**5. What security implications do user namespaces have?**
User namespaces can improve security by isolating user privileges inside a container or isolated environment, reducing the risk of privilege escalation attacks. However, they also require careful configuration to prevent misconfigurations that might allow privileged processes in a namespace to break out and affect the host system. Improper UID/GID mapping or lack of additional namespace isolation can lead to vulnerabilities, which could be exploited by attackers.

**6. Why are other namespace types typically not available to unprivileged users?**
Other namespaces, such as network namespaces or mount namespaces, often require more extensive kernel capabilities that can affect the system at a global level. Allowing unprivileged users to create these namespaces could lead to security vulnerabilities, as they might be able to manipulate system-wide resources like networking or filesystems. Therefore, to prevent abuse, these namespaces are restricted to privileged users, usually requiring root access.

**Conclusion**

Working with user namespaces has provided valuable insights into the flexibility and security mechanisms Linux offers to create isolated environments. It enables processes to have root-like privileges within a controlled context, which is especially useful for container technologies. However, challenges included understanding the intricacies of UID/GID mapping and the limitations on unprivileged users in creating namespaces. Despite these challenges, the practical applications, especially in securing containers, make user namespaces an important tool in modern systems administration and security.


















