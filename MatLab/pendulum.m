%Simulation d'un pendul simple 

%Définitions des paramètres
syms a m g r theta(t) ;
gValue = 9.81;      %g ..
rValue = 0.18;      %Longeur du bras
mValue = 0.300;     %Masse en Kg
kValue = 0.0175;      %Coef force de frottement
omega_0Value = sqrt(gValue/rValue);
%---------D'abord en linéarisant le tout


%Equation du mouvement qui résulte du PFD
eqn = m*a == -g*m*sin(theta);
%On remplace a par l'accélération r * d theta / d t^2
eqn = subs(eqn, a, r*diff(theta, 2));
%On isole l'accélération
eqn = isolate(eqn, diff(theta, 2));
%On met en forme en introduisant la fréquence naturelle du sytème
syms omega_0;
eqn = subs(eqn, g/r, omega_0^2);
%On linéarise
syms x;
taylorApprox = taylor(sin(x), x, 'Order', 2);
taylorApprox = subs(taylorApprox, x, theta);
eqnLineaire = subs(eqn, sin(theta), taylorApprox);
%On résout l'équation une fois linéarisée
syms theta_0 theta_t0;
theta_t = diff(theta);
%Conditions initiales
cond = [theta(0)== theta_0, theta_t(0) == theta_t0];
assume(omega_0, 'real');
thetaSol(t) = dsolve(eqnLineaire, cond);

%--------- Maintenant sans linéariser
syms theta(t) theta_t(t) omega_O k;
%On définit notre système non linéaire de deux équations
eqs = [diff(theta) == theta_t;
    diff(theta_t) == -omega_0^2*sin(theta)-kValue/mValue*theta_t];
%On remplace omega_0 par sa valeur
eqs = subs(eqs, omega_0, omega_0Value);
vars = [theta, theta_t];
%On récupère des matrices pas tout capté
[M, F] = massMatrixForm(eqs, vars);
f = M\F ;
%On récupère une fonction utilisable par matlab
f = odeFunction(f, vars);
%Conditions initiales
x0 = [0.8726; 0];
tInit = 0;
tFinal = 60;
sols = ode45(f, [tInit, tFinal], x0);

%Plot des résulats
figure;

yyaxis left;
plot(sols.x, sols.y(1,:)*180/3.1415);
ylabel('\theta (rad)');

grid on;
title('Closed Path in Phase Space');
xlabel('t (s)');
















