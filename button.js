
const random = (min, max) => 
{
    const rand = min + Math.random() * (max - min + 1);
    return Math.floor(rand);
}


const btn = document.querySelector('#btn');

btn.addEventListener('mouseenter', () => 
{
    btn.style.left = `${random(0, 90)}%`;
    btn.style.top = `${random(0, 90)}%`;
});

document.getElementById('buttonToHeart').addEventListener('click', function() {
    this.classList.toggle('heart');
    if (this.classList.contains('heart')) {
        this.innerText = ':)))';
        this.style.backgroundColor = 'purple';
        this.style.color = 'white';
    } else {
        this.innerText = 'ТАК)';
        this.style.backgroundColor = ''; 
        this.style.color = ''; 
    }
});

const h1Text = "Го гуляти??";


const h1Element = document.querySelector('h1');

// Вставляємо текст у заголовок
h1Element.innerText = h1Text;

