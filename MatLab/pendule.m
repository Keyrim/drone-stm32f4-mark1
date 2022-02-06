%System definition

%Définition des paramètres
syms a m g r theta(t) ;
gValue = 9.81;      %g ..
rPoids = 0.19;      %Longeur pendule
rMoteur = 0.24;     %Longeur bras levier moteur
mValue = 0.312;     %Masse en Kg
kValue = 0.1;      %Coef force de frottement
dt = 0.01 ;

%% Matrice qui décrit la physique du système 
A = [0 1;
    -gValue/rPoids -kValue/mValue];
B = [0; (0.8 * rMoteur) / ((rPoids^2) * mValue)];    %0.0186 car F force du moteur est F(u) = 0.0186*u
C = eye(2);
D = [0; 0];

%% Système parfait
sys = ss(A, B, C, D);
%sysDiscrete = ss(A, B, C, D, dt)
sysDiscrete = c2d(sys, dt);

%Réponse du système à une impultion
%step(sys);

%% Système réel 
Vd = 0.5*eye(2); % Erreur dans le modÃ¨le
Vn = [2 0;
     0 1];     %Erreur dans la mesure
     
BF = [B 0*Vd 0*Vn];
DF = [D 0*Vd Vn];

sysPerfect = ss(A, BF, C, 0*DF);    %Modèle parfait augmented
sysDiscrete = c2d(sysPerfect, dt)
BF = [B Vd 0*Vn];
sysReal = ss(A, BF, C, DF);    %Modèle réel avec erreur dans le modèle et erreur dans la mesure



%% Kalman filter
[Kf, P, E] = lqe(A, Vd, C, Vd, Vn)

sysKF = ss(A-Kf*C, [B Kf], eye(2), 0*[B Kf]);

%% Démo yickes

t = dt:dt:50;

uDIST = randn(2, size(t, 2));
uNOISE = randn(2, size(t, 2));

u = 0*t ;
u(500:5000) = 40 ;

uAUG = [u; Vd*uDIST; Vn*uNOISE];

[y, t] = lsim(sysReal, uAUG, t);
%plot(t, y(:,1));

[x_true, t] = lsim(sysPerfect, uAUG, t);
hold on
plot(t, x_true(:,2), 'r', 'LineWidth', 2.0);
plot(t, x_true(:,1), 'b', 'LineWidth', 2.0);

%u(500:3000) = 0 ;

[x, t] = lsim(sysKF, [u; y'], t);
%plot(t, x(:,1), 'k--', 'LineWidth', 2.00);

hold off
%%

















    