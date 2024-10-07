let number = document.getElementById("guess");
let submit = document.getElementById("submit");
let reset = document.getElementById("reset");
let answer = document.getElementById("answer");
let incorrect = document.getElementById("prev");
let progressbar = document.getElementById("remaining");
let progressBar = document.getElementById('progress');

const A = 1;
const B = 100;
let counts = Math.ceil(Math.log2(B - A + 1));
let current_counts = counts;

number.focus();

function getRandomIntInclusive(min, max) {
  return Math.floor(Math.random() * (max - min + 1) + min);
}

document.getElementById("min").innerText = A;
document.getElementById("max").innerText = B;
document.getElementById("count").innerText = counts;

let random_number = getRandomIntInclusive(A, B);
let incorrectNumbers = [];
progressBar.innerText = current_counts;

submit.addEventListener("click", (event) => {
  event.preventDefault();
  let input_value = Number(document.querySelector(".form-control").value);
  if (input_value < A || input_value > B) {
    answer.innerText = "Выход за пределы!";
  } else if (input_value !== random_number && current_counts > 0) {
    number.value = '';
    current_counts -= 1;
    let remainingPercentage = Math.floor(current_counts / counts * 100);
    progressBar.style.width = remainingPercentage + '%';
    progressBar.innerText = current_counts
    answer.innerText = input_value < random_number ? "Загаданное число БОЛЬШЕ" : "Загаданное число МЕНЬШЕ";
    incorrectNumbers.push(input_value);
    incorrect.innerText = "Использованные ранее числа: " + incorrectNumbers.join(", ");
  } 
  else if (input_value === random_number && current_counts > 0) {
    number.value = '';
    reset.focus();
    answer.innerText = "Вы отгадали загаданное число! Было загадано " + random_number;
    number.disabled = true;
    submit.disabled = true;
  } 
  if (current_counts === 0) {
    reset.focus();
    number.value = '';
    number.disabled = true;
    submit.disabled = true;
    number.style.backgroundColor="#fb6262";
    answer.innerText = "Вы потратили все попытки! Было загадано "+ random_number;
  }
});

reset.addEventListener("click", (event) => {
  location.reload();
});
