Gimmick controls for a cheap chinese "Gaming mouse"

This is still largly work in progress and isn't is only used by me will be working on a gui later 
(gui is complete!)

product id differs
udev rule:

to add the user as a plugdev
sudo usermod -aG plugdev $USER

sudo vim /etc/udev/rules.d/99-fantech-x9.rules then paste :-
SUBSYSTEM=="usb", ATTR{idVendor}=="18f8", ATTR{idProduct}=="0fc0", MODE="0660", GROUP="plugdev"
