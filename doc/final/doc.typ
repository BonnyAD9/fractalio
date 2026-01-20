= Fractalio

== Controls

=== Mouse

*Move around* by pressing the primary mouse button and dragging.

*Zoom* by pressing the secondary mouse button and dragging up and down or by
scrolling.

*Move parameters* by dragging them with primary mouse button.

*Add parameters* by pressing middle click on mouse while not hovering over a
parameter.

*Remove parameters* by pressing middle click on mouse while hovering over a
parameter.

=== Commands

`:`

$
    x_(i+1)&=f(x_i, p, theta) \
    
    x_0 &= 0 \
    x_(i+1) &= x_i^2+p \
    
    x_0 &= p \
    x_(i+1) &= x_i^2 + theta \
    
    x_0 &= 0 \
    x_(i+1) &= x^theta + p \
    
    x_0 &= 0 \
    x_(i+1) &= |Re(x_i^2 + p)| + j|Im(x_i^2 + p)| \
    
    x_0 &= p \
    x_(i+1) &= |Re(x_i^2 + theta)| + j|Im(x_i^2 + theta)| \
    
    x_(i + 1) &= x_i - f(x_i)/(f'(x_i)) \
    
    theta''_1&=(
        - g(2m_1 + m_2)sin(theta_1)
        - m_2g sin(theta_1-2theta_2)
        - 2sin(theta_1-theta_2)m_2(L_2theta'_2^2+L_1theta'_1^2cos(theta_1-theta_2))
    ) / (
        L_1(2m_1+m_2(1-cos(2(theta_1-theta_2))))
    ) \
    theta''_2 &= (2sin(theta_1-theta_2)(
        L_1theta'_1^2(m_1+m_2)
        + cos(theta_1)g(m_1+m_2)
        + L_2m_2theta'_2^2cos(theta_1+theta_2)
    )) / (
        L_2(2m_1+m_2(1-cos(2(theta_1-theta_2))))
    ) \
    
    arrow(a)''_i &= sum_j^(j!=i)(arrow(a)_j-arrow(a)_i)(g m_j)/(|arrow(a)_j-arrow(a)_i|^2 + epsilon) \
    
    arrow(a)'' &= sum_j^(N)(arrow(a)_j-arrow(a))(g m_j)/(|arrow(a)_j-arrow(a)|^2 + epsilon) \
    
    f(x)&=c_0+c_1x+c_2x^2+...+c_n x^n \
    r_k_(i+1) &= r_k_i - (f(r_k_i)/(f'(r_k_i)))/(
        1-f(r_k_i)/(f'(r_k_i))
        sum_j^(j!=k)1/(r_k_i-r_j_i)
    )
$