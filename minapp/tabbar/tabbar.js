// pages/tabbar/tabbar.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    listInfo: [{
      text: '首页',
      imgUrl: '/pages/images/index.png',
      curUrl: '/pages/images/index-active.png',
    },
    {
      text: '服务',
      imgUrl: '/pages/images/service.png',
      curUrl: '/pages/images/service-active.png',
    },
    {
      text: '我的',
      imgUrl: '/pages/images/mine.png',
      curUrl: '/pages/images/mine-active.png',
    }]
  },
  // 底部导航
  chooseImg: function (e) {
    var that = this
    this.setData({
      curIdx: e.currentTarget.dataset.current
    })
    console.log("kk", e.currentTarget.dataset.current)
    if (e.currentTarget.dataset.current == 0) {
      wx.switchTab({
        url: '/pages/index/index',
      })
    } else if (e.currentTarget.dataset.current == 1) {
      wx.switchTab({
        url: '/pages/service/service',
      })
    } else if (e.currentTarget.dataset.current == 2) {
      wx.switchTab({
        url: '/pages/mine/mine',
      })
    }
    that.onShow();
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
    this.onLoad();
  },
})
