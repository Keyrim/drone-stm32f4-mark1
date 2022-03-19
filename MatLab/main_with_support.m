%% Paramètres physique du modèle
% Matrice d'inertie du drone en g.mm^2
format longg
I = [1.985E+06 62.968	-2454.008;
	62.968 2.330E+06 12.322;
	-2454.008 12.322 3.242E+06];
% Matrice d'inertie du drone kg.m^2
I = I * 1E-9;
% Inverse de la matrice d'inertie du drone
iInv = inv(I);
% Coef force de frottement quand le drone tourne en kg*m*s^-2 / s * rad
f = 0.17;
% Conversion signal moteur =>force en Newton(kg*m*s^-2)
k = 0.008;
% Constance de couple en yaw
C = 0.0003;
% Position du centre de gravité du drone en mm (converti après en m)
centerOfMass = [-16.65 -51.621 121.177];
centerOfMass = centerOfMass * 0.001; %conversion mm
% Positions des moteurs en mm (converti plus tard en mm)
motorsPositionAbsolute = [52.021 38.063 107.289;
    52.022 -141.312 107.289;
   -85.456 -141.312 107.289;
   -85.456 38.064 107.289];
motorsPositionAbsolute = motorsPositionAbsolute * 0.001; %conversion mm
% Positions des moteurs relativement au centre de gravité du drone
mPos = motorsPositionAbsolute - centerOfMass
% Matrice pour obtenir les moments générer par les moteurs
M = k*[mPos(1, 2) mPos(2, 2) mPos(3, 2) mPos(4, 2);
    -mPos(1, 1) -mPos(2, 1) -mPos(3, 1) -mPos(4, 1)
    -C/k C/k -C/k C/k]
dt = 0.001;

%% Définition des matrices du système
A = [-f    0       0;
    0       -f     0;
    0       0       -f];
B = iInv * M;
C = eye(3);
D = zeros(3, 4);
sys = ss(A, B, C, D);

dis_sts = c2d(sys, 0.001)

%% open loop simulation to step impulse
t = dt:dt:120;
u = [0*t.' 0*t.' 0*t.' 0*t.'];
aza = 000;
u(2400:aza,1) = 10;
u(2400:aza,4) = 10;
[x, t] = lsim(dis_sts, u, t);

hold on
plot(t, x(:,1), 'r', 'LineWidth', 2.0);
%plot(t, x(:,2), 'b', 'LineWidth', 2.0);
%plot(t, x(:,3), 'g', 'LineWidth', 2.0);

hold off


