clear
clc
x=reshape((0:23)*(1-1j),[4,6]).';
 my_dwt2(x);

function [a,h,v,d] = my_dwt2(x)
Lo_D = [sqrt(0.5),sqrt(0.5)];
Hi_D = [-sqrt(0.5),sqrt(0.5)];
shift = [0,0];
lf = 2;
sx = size(x);
first = 2-shift;
last = sx+lf-1;
x = double(x);
if length(sx)==2    % 1-channel

    y = [x(:,1),x,x(:,sx(2))];
    z = zeros(size(y,1),size(y,2)-1);
    for ii = 1:size(z,2)
        z(:,ii) = y(:,ii:ii+1)*Lo_D(:);
    end
    
    a = convdown(z,Lo_D,first,last);
    h = convdown(z,Hi_D,first,last);
    z = zeros(size(y,1),size(y,2)-1);
    for ii = 1:size(z,2)
        z(:,ii) = y(:,ii:ii+1)*Hi_D(:);
    end
    
    v = convdown(z,Lo_D,first,last);
    d = convdown(z,Hi_D,first,last);
% else                % 3-channels
%     a = cell(0,3);
%     h = cell(0,3);
%     v = cell(0,3);
%     d = cell(0,3);
%     for k = 1:3
%         y = wextend('addcol',dwtEXTM,x(:,:,k),sizeEXT);
%         z = conv2(y,Lo_D(:)','valid');
%         a{k} = convdown(z,Lo_D,dwtEXTM,sizeEXT,first,last);
%         h{k} = convdown(z,Hi_D,dwtEXTM,sizeEXT,first,last);
%         z = conv2(y,Hi_D(:)','valid');
%         v{k} = convdown(z,Lo_D,dwtEXTM,sizeEXT,first,last);
%         d{k} = convdown(z,Hi_D,dwtEXTM,sizeEXT,first,last);
%     end
%     a = cat(3,a{:});
%     h = cat(3,h{:});
%     v = cat(3,v{:});
%     d = cat(3,d{:});
end
end
%-------------------------------------------------------%
% Internal Function(s)
%-------------------------------------------------------%
function y = convdown(x,F,first,last)
% y = convdown(x,F,dwtEXTM,lenEXT,first,last)
y = x(:,first(2):2:last(2));
y = [y(1,:);y;y(size(y,1),:)];
% y = wextend('addrow',dwtEXTM,y,lenEXT); [row,col] = size(y); y1 = y'; z =
% zeros(col,row-1); for ii = 1:size(z,2)
%   z(:,ii) = y1(:,ii:ii+1)*F(:);
% end y = conv2(y',F(:)','valid'); sum((y-z)>1e-10,'all')
[row,col] = size(y);
y = y';
z = zeros(col,row-1);
for ii = 1:size(z,2)
  z(:,ii) = y(:,ii:ii+1)*F(:);
end
y = z';
y = y(first(1):2:last(1),:);
%-------------------------------------------------------%
end