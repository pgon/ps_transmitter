close all; clear all 

x = csvread ('datos_deep_sleep_solo_baterias.csv');
t = (x(:,2)-x(1,2))/3600;
pres = x(:,3);
%temp = x(:,4);
%latencia = x(:,5)/1000;
dt = [30;(t(2:end)-t(1:end-1))*3600];
hf=figure()
subplot(2,2,1); plot(t,pres); ylabel('Presion [mm Hg]');grid on; xlabel('Tiempo [hs]')
title('Deep Sleep - Baterias') 
%subplot(2,2,2); plot(t,temp); ylabel('Temperatura [?]');grid on; xlabel('Tiempo [hs]')
%subplot(2,2,3); plot(t,latencia); ylabel('Latencia [s]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,4); plot(t,dt); ylabel('\Delta T [s]');grid on; xlabel('Tiempo [hs]')
print -dpdf  "datos_deep_sleep_solo_baterias.pdf"


x = csvread ('datos_deep_sleep_solo_baterias_sin_usb.csv');
t = (x(:,2)-x(1,2))/3600;
pres = x(:,3);
temp = x(:,4);
latencia = x(:,5)/1000;
dt = [30;(t(2:end)-t(1:end-1))*3600];
figure()
subplot(2,2,1); plot(t,pres); ylabel('Presion [mm Hg]');grid on; xlabel('Tiempo [hs]')
title('Deep Sleep - Baterias - Sin USB') 
subplot(2,2,2); plot(t,temp); ylabel('Temperatura [?]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,3); plot(t,latencia); ylabel('Latencia [s]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,4); plot(t,dt); ylabel('\Delta T [s]');grid on; xlabel('Tiempo [hs]')
print -dpdf  "datos_deep_sleep_solo_baterias_sin_usb.pdf"


x = csvread ('datos_modem_sleep_bateria.csv');
t = (x(:,2)-x(1,2))/3600;
pres = x(:,3);
temp = x(:,4);
latencia = x(:,5)/1000;
dt = [30;(t(2:end)-t(1:end-1))*3600];
figure()
subplot(2,2,1); plot(t,pres); ylabel('Presion [mm Hg]');grid on; xlabel('Tiempo [hs]')
title('Modem Sleep - Batería') 
subplot(2,2,2); plot(t,temp); ylabel('Temperatura [?]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,3); plot(t,latencia); ylabel('Latencia [s]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,4); plot(t,dt); ylabel('\Delta T [s]');grid on; xlabel('Tiempo [hs]')
print -dpdf  "datos_modem_sleep_bateria.pdf"


x = csvread ('datos_modem_sleep_cargando2.csv');
t = (x(:,2)-x(1,2))/3600;
pres = x(:,3);
temp = x(:,4);
latencia = x(:,5)/1000;
dt = [30;(t(2:end)-t(1:end-1))*3600];
figure()
subplot(2,2,1); plot(t,pres); ylabel('Presion [mm Hg]');grid on; xlabel('Tiempo [hs]')
title('Modem Sleep - Cargando') 
subplot(2,2,2); plot(t,temp); ylabel('Temperatura [?]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,3); plot(t,latencia); ylabel('Latencia [s]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,4); plot(t,dt); ylabel('\Delta T [s]');grid on; xlabel('Tiempo [hs]')
print -dpdf  "datos_modem_sleep_cargando2.pdf"

x = csvread ('datos_modem_sleep_cargando2.csv');
t = (x(:,2)-x(1,2))/3600;
pres = x(:,3);
temp = x(:,4);
latencia = x(:,5)/1000;
dt = [30;(t(2:end)-t(1:end-1))*3600];
figure()
subplot(2,2,1); plot(t,pres); ylabel('Presion [mm Hg]');grid on; xlabel('Tiempo [hs]')
title('Modem Sleep - Cargando') 
subplot(2,2,2); plot(t,temp); ylabel('Temperatura [?]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,3); plot(t,latencia); ylabel('Latencia [s]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,4); plot(t,dt); ylabel('\Delta T [s]');grid on; xlabel('Tiempo [hs]')
print -dpdf  "datos_modem_sleep_cargando2.pdf"

x = csvread ('datos_sin_dormir_mirar_dispersion_al final.csv');
t = (x(:,2)-x(1,2))/3600;
pres = x(:,3);
temp = x(:,4);
latencia = x(:,5)/1000;
dt = [30;(t(2:end)-t(1:end-1))*3600];
figure()
subplot(2,2,1); plot(t,pres); ylabel('Presion [mm Hg]');grid on; xlabel('Tiempo [hs]')
title('No Sleep - Baterías') 
subplot(2,2,2); plot(t,temp); ylabel('Temperatura [?]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,3); plot(t,latencia); ylabel('Latencia [s]');grid on; xlabel('Tiempo [hs]')
subplot(2,2,4); plot(t,dt); ylabel('\Delta T [s]');grid on; xlabel('Tiempo [hs]')
print -dpdf  "datos_sin_dormir_mirar_dispersion_al final.pdf"
