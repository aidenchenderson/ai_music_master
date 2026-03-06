const toggleSwitch = document.getElementById("darkModeSwitch");
const logo = document.getElementById("siteLogo");
const defaultTheme = "light";

const logos = {
  light: "assets/logo-light.svg",
  dark: "assets/logo-dark.svg"
};

function themeChange(theme) {
  document.documentElement.setAttribute("data-theme", theme);
  if (logo) {
    logo.style.opacity = 0;
    setTimeout(() => {
      logo.src = logos[theme];
      logo.style.opacity = 1;
    }, 150);
  }
}

function getStartMode() {
  const saved = localStorage.getItem("theme");
  if (saved === "dark" || saved === "light") return saved;
  return defaultTheme;
}

themeChange(getStartMode());

toggleSwitch.addEventListener("click", () => {
  const currentTheme = document.documentElement.getAttribute("data-theme") || defaultTheme;
  const nextTheme = (currentTheme === "dark") ? "light" : "dark";
  const icon = toggleSwitch.querySelector("i");

  icon.style.transform = "rotate(360deg)";
  setTimeout(() => { icon.style.transform = "rotate(0deg)"; }, 600);

  themeChange(nextTheme);
  localStorage.setItem("theme", nextTheme);
});

const slideImg = document.getElementById("slideImage");
const prevBtn = document.getElementById("prevBtn");
const nextBtn = document.getElementById("nextBtn");

const images = [
  "images/slide1.jpg",
  "images/slide2.png",
  "images/slide3.png",
];

let index = 0;

function slideDisplay(i) {
  if (!slideImg || images.length === 0) return;
  index = (i + images.length) % images.length;
  slideImg.style.opacity = 0;
  setTimeout(() => {
    slideImg.src = images[index];
    slideImg.style.opacity = 1;
  }, 300);
}

slideDisplay(0);
prevBtn.addEventListener("click", () => slideDisplay(index - 1));
nextBtn.addEventListener("click", () => slideDisplay(index + 1));