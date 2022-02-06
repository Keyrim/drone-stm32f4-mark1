%% Paramètres physique du modèle
% Matrice d'inertie du drone en g.mm^2
I = [1.862E+06 43.454	1.549E+05;
	43.454 1.593E+06 23.054;
	1.549E+05 23.054 2.606E+06];
% Inverse de la matrice d'inertie du drone
iInv = inv(I);
% Coef force de frottement quand le drone tourne
f = 0.8;
% Conversion signal moteur =>force en Newton
k = 10;
% Adaption de la force en Newton à une force en mm*g/s^2
k = k*1E+06;
% Constance de couple en yaw
C = 10;
% Position du centre de gravité du drone
centerOfMass = [49.089 14.532 27.805];
% Positions des moteurs en mm
motorsPositionAbsolute = [126.89 104.21 9.599;
    126.89 -75.16 9.599;
    -10.58 -75.16 9.599;
    -10.58 104.21 9.599];
% Positions des moteurs relativement au centre de gravité du drone
mPos = motorsPositionAbsolute - centerOfMass;
% Matrice pour obtenir les moments générer par les moteurs
M = k*[mPos(1, 2) mPos(2, 2) mPos(3, 2) mPos(4, 2);
    mPos(1, 1) mPos(2, 1) mPos(3, 1) mPos(4, 1)
    -C C -C C];
dt = 0.01;

%% Définition des matrices du système
A = [-f    0       0;
    0       -f     0;
    0       0       -f];
B = iInv * M;
C = eye(3);
D = zeros(3, 4);
sys = ss(A, B, C, D);

%% open loop simulation to step impulse
t = dt:dt:20;
u = [0*t.' 0*t.' 0*t.' 0*t.'];
u(100:1000,1:3) = 0.1;
[x, t] = lsim(sys, u, t);

hold on
plot(t, x(:,1), 'r', 'LineWidth', 2.0);
plot(t, x(:,2), 'b', 'LineWidth', 2.0);
plot(t, x(:,3), 'g', 'LineWidth', 2.0);

hold off




