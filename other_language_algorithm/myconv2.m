clear
clc
x=reshape((0:23)*(1-1j),[4,6]).';
sx = size(x);
Lo_D = [sqrt(0.5),sqrt(0.5)];
Hi_D = [-sqrt(0.5),sqrt(0.5)];
Lo_D = Lo_D.*[1-1j,1+1j];

% y = [x(:,1),x,x(:,sx(2))];
% z = zeros(size(y,1),size(y,2)-1);
% for ii = 1:size(z,2)
%   z(:,ii) = y(:,ii:ii+1)*Lo_D(:);
% end

y = [x(1,:);x;x(sx(1),:)];
z = zeros(size(y,1)-1,size(y,2));
for ii = 1:size(z,1)
  z(ii,:) = y(ii:ii+1,:).'*Lo_D(:);
end