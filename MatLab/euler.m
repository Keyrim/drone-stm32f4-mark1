point1 = [1,0,0];
point2 = [0,1,0];
point3 = [0,0,1];
origin = [0,0,0];
figure;hold on;
plot3([origin(1) point1(1)],[origin(2) point1(2)],[origin(3) point1(3)],'r-^', 'LineWidth',3);
plot3([origin(1) point2(1)],[origin(2) point2(2)],[origin(3) point2(3)],'g-^', 'LineWidth',3);
plot3([origin(1) point3(1)],[origin(2) point3(2)],[origin(3) point3(3)],'b-^', 'LineWidth',3);
grid on;
xlabel('X axis'), ylabel('Y axis'), zlabel('Z axis')
set(gca,'CameraPosition',[1 2 3]);