%% Paramètres physique du modèle
% Matrice d'inertie du drone en g.mm^2
format longg
I = [1.862E+06 43.454	1.549E+05;
	43.454 1.593E+06 23.054;
	1.549E+05 23.054 2.606E+06];
% Matrice d'inertie du drone kg.m^2
I = I * 1E-9
% Inverse de la matrice d'inertie du drone
iInv = inv(I);
% Coef force de frottement quand le drone tourne en kg*m*s^-2 / s * rad
f = 0.8;
% Conversion signal moteur =>force en Newton(kg*m*s^-2)
k = 1;
% Constance de couple en yaw
C = 0.01;
% Position du centre de gravité du drone en m
centerOfMass = [0.049089 0.014532 0.027805];
% Positions des moteurs en m
motorsPositionAbsolute = [0.12689 0.10421 0.009599;
    0.12689 -0.07516 0.009599;
    -0.1058 -0.07516 0.009599;
    -0.1058 0.10421 0.009599];
% Positions des moteurs relativement au centre de gravité du drone
mPos = motorsPositionAbsolute - centerOfMass;
% Matrice pour obtenir les moments générer par les moteurs
M = k*[mPos(1, 2) mPos(2, 2) mPos(3, 2) mPos(4, 2);
    -mPos(1, 1) -mPos(2, 1) -mPos(3, 1) -mPos(4, 1)
    -C/k C/k -C/k C/k];
dt = 0.01;

%% Définition des matrices du système
A = [-f    0       0;
    0       -f     0;
    0       0       -f];
B = iInv * M
C = eye(3);
D = zeros(3, 4);
sys = ss(A, B, C, D);

dis_sts = c2d(sys, 0.001)

%% open loop simulation to step impulse
t = dt:dt:20;
u = [0*t.' 0*t.' 0*t.' 0*t.'];
u(100:1000,1) = 0.1;
u(100:1000,2) = 0.1;
[x, t] = lsim(sys, u, t);

hold on
plot(t, x(:,1), 'r', 'LineWidth', 2.0);
plot(t, x(:,2), 'b', 'LineWidth', 2.0);
plot(t, x(:,3), 'g', 'LineWidth', 2.0);

hold off




