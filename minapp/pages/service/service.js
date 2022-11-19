// pages/service/service.js
Page({

  /**a
   * 页面的初始数据
   */
  data: {
    mp4Address:"https://apd-520f074e5217d3295e8bc73ed490a8ec.v.smtcdns.com/om.tc.qq.com/A5Nf7zbK8JgoxjUVSNbNZes2F9yUKjgu6DH4cSGqW4_A/B_ppPqCIXz9JbN8GvwkO8ikQ/svp_50001/szg_62859730_50001_af5996f925f54800b90610478d218b2d.f632.mp4?sdtfrom=v1010&guid=c055f3e9a05db520&vkey=362EEE8131897CDC845E666F0F3B0A413C81573AA7AFE0AC100987D37396F1FFDB93C2107785C4FE9E7DAC26C5DF8F1CF35CDC284BB28AF629F66C437D563AD1C14870126880772490312FDC342111113DE103EAD4457A725A3E51163242A049833E20F6878E50C42AD23A6B9510C1F91CFAC9F247CEF9F440FF95E292A3A9FF",
    swiper_item: [
      { page: 'tips/tips', image: '/images/swiper-item-1.jpg' },
      { page: 'latest/latest', image: '/images/swiper-item-2.jpg' },
      { page: 'policy/policy', image: '/images/swiper-item-3.jpg' },
    ],
    //tabbar
    listInfo: [{
      text: '首页',
      imgUrl: '/images/index.png',
      curUrl: '/images/index-active.png',
    },
    {
      text: '服务',
      imgUrl: '/images/service.png',
      curUrl: '/images/service-active.png',
    },
    {
      text: '我的',
      imgUrl: '/images/mine.png',
      curUrl: '/images/mine-active.png',
    }],
  },
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
    } else if (e.currentTarget.dataset.current == 3) {
      wx.switchTab({
        url: '/pages/person/person',
      })
    }
    that.onShow();
  },
  //点击事件：goTo，跳转api
  goTo(e) {
    var url = e.currentTarget.dataset.id
    wx.navigateTo({
      url: url,
    })
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    var curIdx = 1
    this.setData({
      curIdx: curIdx,
    })


  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {
    this.onLoad();
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload() {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom() {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage() {

  }
})