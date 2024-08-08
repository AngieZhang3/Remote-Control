# About
A MFC-based application that provides the following remote control features:
1. View the screen remotely, control the mouse, and control the keyboard.
2. Execute CMD commands remotely and display the results on the local machine.
3. View, upload, and download files remotely.

# Use
## Connect
### Server
1. Click **Connect**.
2. Type in your local machine's IP and Port.
   <img width="717" alt="Server login" src="https://github.com/user-attachments/assets/2a95463c-6879-4505-a2a9-3f6f7ae4fe7d">

### Client
1. Type in IP and Port.
2. Click **Connect**

<img width="329" alt="client login" src="https://github.com/user-attachments/assets/32742399-4c5b-433c-94d0-49582b178073">

## View Remote Desktop
### Server
- Click **View Desktop** to see remote machine's desktop
- Click **Mouse Control**-> **start** to take control of remote machine's mouse 
- Click **Keyboard Control**-> **start** to take control of remote machine's keyboard
- Click **Stop Desktop** to stop viewing remote machine's desktop. The mouse and keyboard control will end once the desktop view is stopped.
- Click **Mouse Control**->**stop** to stop mouse control 
- Click **Keyboard Control**-> **stop** to stop keyboard control
  <img width="1788" alt="desktop" src="https://github.com/user-attachments/assets/d371ba9f-c60f-4cd2-b269-89cb79d24cce">

## Execute CMD Command
### Server
- Click **Execute CMD**
- Click **Start**
- Type in command and click **Execute**
![image](https://github.com/user-attachments/assets/b131aa98-12ff-40fb-9b64-5aa372af1cbd)


## Files 
### Server
- Click **Files**
- A file explorer will show up, allowing you to browese the remote machines' files
- Right-click a file or file folder, and choose to download or upload files.
  ![image](https://github.com/user-attachments/assets/dc154793-1ef8-43f9-82cd-0c22fd3812b3)
## Disconnect
### Server
- Click **Disconnect**
### Client
- Click **Stop**

# Issues
Stop the desktop view before using the Execute CMD or Files functions. 
